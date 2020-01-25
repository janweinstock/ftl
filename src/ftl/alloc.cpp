/******************************************************************************
 *                                                                            *
 * Copyright 2019 Jan Henrik Weinstock                                        *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License");            *
 * you may not use this file except in compliance with the License.           *
 * You may obtain a copy of the License at                                    *
 *                                                                            *
 *     http://www.apache.org/licenses/LICENSE-2.0                             *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 *                                                                            *
 ******************************************************************************/

#include "ftl/alloc.h"

namespace ftl {

    alloc::alloc(emitter& e):
        m_emitter(e),
        m_regmap(),
        m_usecnt(0),
        m_locals(~0ull),
        m_base(0),
        m_values(),
        m_blacklist() {
        reset();
    }

    alloc::~alloc() {
        // nothing to do
    }

    void alloc::blacklist(reg r) {
        if (!is_blacklisted(r))
            m_blacklist.push_back(r);
    }

    void alloc::unblacklist(reg r) {
        if (is_blacklisted(r))
            stl_remove_erase(m_blacklist, r);
    }

    bool alloc::is_blacklisted(reg r) const {
        return stl_contains(m_blacklist, r);
    }

    bool alloc::is_empty(reg r) const {
        FTL_ERROR_ON(!reg_valid(r), "invalid register specified");
        if (m_regmap[r].owner == nullptr)
            return true;
        if (m_regmap[r].owner->is_dead())
            return true;
        return false;
    }

    bool alloc::is_dirty(reg r) const {
        FTL_ERROR_ON(!reg_valid(r), "invalid register specified");
        if (is_empty(r))
            return false;
        return m_regmap[r].dirty;
    }

    void alloc::mark_dirty(reg r) {
        FTL_ERROR_ON(!reg_valid(r), "invalid register specified");
        m_regmap[r].dirty = true;
    }

    void alloc::mark_clean(reg r) {
        FTL_ERROR_ON(!reg_valid(r), "invalid register specified");
        m_regmap[r].dirty = false;
    }

    // Never allocate RSP or RBP, since we need them for addressing local and
    // memory variables. Allocate RAX last, since we need it to return values
    // from function calls. RDX gets spoiled in MUL/DIV operations. Prefer
    // registers which are callee saved and not used for function arguments.
    static const array<reg, 14> alloc_order = {
        RBP, R12, R13, R14, R15, R8, R9, R10, R11, RCX, RDX, RSI, RDI, RAX,
    };

    reg alloc::select() const {
        vector<reg> regs;
        std::copy_if(alloc_order.begin(), alloc_order.end(),
                     std::back_inserter(regs), [this](reg r) -> bool {
            return !is_blacklisted(r);
        });

        // try unused registers first
        for (reg r : regs)
            if (is_empty(r))
                return r;

        // next, try registers that do not need to be flushed
        for (reg r : regs)
            if (!is_dirty(r))
                return r;

        // pick least recently used
        reg lru = NREGS;
        u64 min = ~0ull;
        for (reg r : regs) {
            if (m_regmap[r].count < min) {
                min = m_regmap[r].count;
                lru = r;
            }
        }

        FTL_ERROR_ON(lru == NREGS, "failed to select a register");
        return lru;
    }

    reg alloc::lookup(const value* val) const {
        if (val == nullptr)
            return NREGS;

        for (auto info : m_regmap) {
            if (info.owner == val) {
                info.count = m_usecnt++;
                return info.regid;
            }
        }

        return NREGS;
    }

    reg alloc::assign(const value* val, reg r) {
        FTL_ERROR_ON(!val, "attempt to assign nullptr value");

        if (r == NREGS)
            r = lookup(val);
        if (r == NREGS)
            r = select();

        FTL_ERROR_ON(!reg_valid(r), "invalid register selected: %d", r);
        FTL_ERROR_ON(r == STACK_POINTER, "cannot assign to stack pointer");
        FTL_ERROR_ON(r == BASE_REGISTER, "cannot assign to base register");
        FTL_ERROR_ON(is_blacklisted(r), "cannot assign to blacklist register");

        if (m_regmap[r].owner == val)
            return r;

        flush(r);

        reg curr = lookup(val);
        if (curr < NREGS)
            free(curr);

        m_regmap[r].owner = val;
        m_regmap[r].dirty = false;
        m_regmap[r].count = m_usecnt++;

        return r;
    }

    reg alloc::fetch(const value* val, reg r) {
        FTL_ERROR_ON(!val, "attempt to fetch nullptr value");

        reg curr = lookup(val);
        if ((curr < NREGS) && (curr == r || r == NREGS))
            return curr;

        r = assign(val, r);

        if (curr < NREGS && curr != r) {
            m_emitter.movr(val->bits, r, curr);
        } else {
            FTL_ERROR_ON(val->is_scratch(), "attempt to fetch scratch value");
            if (!val->is_directly_addressable()) {
                reg base = select();
                flush(base);
                m_emitter.movi(64, base, val->addr);
                m_emitter.movr(val->bits, r, memop(base, 0));
            } else {
                m_emitter.movr(val->bits, r, val->mem);
            }
        }

        return r;
    }

    void alloc::free(reg r) {
        FTL_ERROR_ON(!reg_valid(r), "invalid register specified");

        m_regmap[r].owner = nullptr;
        m_regmap[r].dirty = false;
        m_regmap[r].count = 0;
    }

    void alloc::store(reg r) {
        FTL_ERROR_ON(!reg_valid(r), "invalid register specified");

        if (!is_dirty(r))
            return;

        const value* val = m_regmap[r].owner;
        FTL_ERROR_ON(val == nullptr, "store operation on empty register");

        if (val->is_scratch())
            return;

        if (!val->is_directly_addressable()) {
            reg base = select();
            flush(base);
            m_emitter.movi(64, base, val->addr);
            m_emitter.movr(val->bits, memop(base, 0), r);
        } else {
            m_emitter.movr(val->bits, val->mem, r);
        }

        mark_clean(r);
    }

    void alloc::flush(reg r) {
        FTL_ERROR_ON(!reg_valid(r), "invalid register specified");
        store(r);
        free(r);
    }

    void alloc::register_value(value* val) {
        if (stl_contains(m_values, val))
            FTL_ERROR("attempt to register value '%s' twice", val->name());
        m_values.push_back(val);
    }

    void alloc::unregister_value(value* val) {
        if (!stl_contains(m_values, val))
            FTL_ERROR("attempt to unregister unknown value '%s'", val->name());
        stl_remove_erase(m_values, val);
    }

    value alloc::new_local_noinit(const string& name, int bits, reg r) {
        int idx = ffs(m_locals) - 1;
        FTL_ERROR_ON(idx < 0, "out of stack frame memory");
        m_locals &= ~(1ull << idx);

        if (r == NREGS)
            r = select();

        value v(*this, name, bits, true, 0, STACK_POINTER, -idx * sizeof(u64));

        flush(r);
        assign(&v, r);

        return v;
    }

    value alloc::new_local(const string& name, int bits, i64 val, reg r) {
        value v = new_local_noinit(name, bits, r);
        r = v.r();
        m_emitter.movi(bits, r, val);
        mark_dirty(r);
        return v;
    }

    value alloc::new_global(const string& name, int bits, u64 addr) {
        if (m_base == 0) {
            m_base = FTL_PAGE_ROUND(addr + FTL_PAGE_SIZE);
            m_emitter.movi(64, BASE_REGISTER, m_base);
        }

        i32 offset = addr - m_base;
        value v(*this, name, bits, true, addr, BASE_REGISTER, offset);
        return v;
    }

    value alloc::new_scratch_noinit(const string& name, int bits, reg r) {
        if (r == NREGS)
            r = select();
        flush(r);

        value v(*this, name, bits, true, ~0ull, NREGS, 0);
        assign(&v, r);
        return v;
    }

    value alloc::new_scratch(const string& name, int bits, i64 val, reg r) {
        value v = new_scratch_noinit(name, bits, r);
        r = v.r();
        m_emitter.movi(bits, r, val);
        mark_dirty(r);
        return v;
    }

    void alloc::free_value(value& val) {
        FTL_ERROR_ON(val.is_dead(), "double free value %s", val.name());

        if (val.is_local()) {
            int idx = -val.mem.offset / sizeof(u64);
            FTL_ERROR_ON(idx < 0 || idx > 64, "corrupt stack offset");
            m_locals |= (1 << idx);
        }

        reg r = lookup(&val);
        if (r < NREGS)
            free(r);

        val.mark_dead();
    }

    size_t alloc::count_dirty_regs() const {
        size_t count = 0;
        for (auto reg : m_regmap)
            if (reg.owner != nullptr && reg.dirty)
                count++;
        return count;
    }

    size_t alloc::count_active_regs() const {
        size_t count = 0;
        for (auto reg : m_regmap)
            if (reg.owner != nullptr)
                count++;
        return count;
    }

    void alloc::store_all_regs() {
        for (reg r : all_regs)
            store(r);
    }

    void alloc::flush_all_regs() {
        for (reg r : all_regs)
            flush(r);
    }

    void alloc::store_volatile_regs() {
        for (reg r : caller_saved_regs)
            store(r);
    }

    void alloc::flush_volatile_regs() {
        for (reg r : caller_saved_regs)
            flush(r);
    }

    void alloc::reset() {
        for (auto val : m_values)
            val->mark_dead();

        m_values.clear();
        m_locals = ~0ull;
        m_usecnt = 0;

        for (auto r : all_regs) {
           m_regmap[r].regid = r;
           m_regmap[r].owner = nullptr;
           m_regmap[r].dirty = false;
           m_regmap[r].count = 0;
       }
    }

}
