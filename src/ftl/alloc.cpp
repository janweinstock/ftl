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
        m_values() {
        reset();
    }

    alloc::~alloc() {
        // nothing to do
    }

    bool alloc::is_empty(reg r) const {
        FTL_ERROR_ON(!reg_valid(r), "invalid register specified");
        if (m_regmap[r].owner == NULL)
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
    static const reg alloc_order[] = {
        RBP, R12, R13, R14, R15, R8, R9, R10, R11, RCX, RDX, RSI, RDI, RAX,
    };

    reg alloc::select() const {
        // try unused registers first
        for (reg r : alloc_order)
            if (is_empty(r))
                return r;

        // next, try registers that do not need to be flushed
        for (reg r : alloc_order)
            if (!is_dirty(r))
                return r;

        // pick least recently used
        reg lru = NREGS;
        u64 min = ~0ull;
        for (reg r : alloc_order) {
            if (m_regmap[r].count < min) {
                min = m_regmap[r].count;
                lru = r;
            }
        }

        FTL_ERROR_ON(lru == NREGS, "failed to select a register");
        return lru;
    }

    reg alloc::lookup(const value* val) const {
        if (val == NULL)
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
        FTL_ERROR_ON(!val, "attempt to assign NULL value");

        if (r == NREGS)
            r = lookup(val);
        if (r == NREGS)
            r = select();

        FTL_ERROR_ON(!reg_valid(r), "invalid register selected: %d", r);
        FTL_ERROR_ON(r == STACK_POINTER, "cannot assign to stack pointer");
        FTL_ERROR_ON(r == BASE_REGISTER, "cannot assign to base register");

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
        FTL_ERROR_ON(!val, "attempt to fetch NULL value");

        reg curr = lookup(val);
        if ((curr < NREGS) && (curr == r || r == NREGS))
            return curr;

        r = assign(val, r);

        if (curr < NREGS && curr != r) {
            m_emitter.movr(val->bits, r, curr);
        } else {
            if (!val->is_directly_addressable()) {
                m_emitter.movi(64, BASE_REGISTER, val->addr);
                m_emitter.movr(val->bits, r, memop(BASE_REGISTER, 0));
                m_emitter.movi(64, BASE_REGISTER, m_base);
            } else {
                m_emitter.movr(val->bits, r, val->mem);
            }
        }

        return r;
    }

    void alloc::free(reg r) {
        FTL_ERROR_ON(!reg_valid(r), "invalid register specified");
        m_regmap[r].owner = NULL;
        m_regmap[r].dirty = false;
        m_regmap[r].count = 0;
    }

    void alloc::store(reg r) {
        FTL_ERROR_ON(!reg_valid(r), "invalid register specified");

        if (!is_dirty(r))
            return;

        const value* val = m_regmap[r].owner;
        FTL_ERROR_ON(val == NULL, "store operation on empty register");

        if (!val->is_directly_addressable()) {
            m_emitter.movi(64, BASE_REGISTER, val->addr);
            m_emitter.movr(val->bits, memop(BASE_REGISTER, 0), r);
            m_emitter.movi(64, BASE_REGISTER, m_base);
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

    value alloc::new_local_noinit(const string& name, int bits, bool sign,
                                  reg r) {
        int idx = ffs(m_locals) - 1;
        FTL_ERROR_ON(idx < 0, "out of stack frame memory");
        m_locals &= ~(1ull << idx);

        if (r == NREGS)
            r = select();
        flush(r);

        value v(*this, name, bits, sign, 0, STACK_POINTER, -idx * sizeof(u64));
        assign(&v, r);
        return v;
    }

    value alloc::new_local(const string& name, int bits, bool sign, i64 val,
                           reg r) {
        value v = new_local_noinit(name, bits, sign, r);
        r = v.r();
        m_emitter.movi(bits, r, val);
        mark_dirty(r);
        return v;
    }

    value alloc::new_global(const string& name, int bits, bool sign, u64 addr) {
        if (m_base == 0) {
            m_base = FTL_PAGE_ROUND(addr + FTL_PAGE_SIZE);
            m_emitter.movi(64, BASE_REGISTER, m_base);
        }

        i32 offset = addr - m_base;
        value v(*this, name, bits, sign, addr, BASE_REGISTER, offset);

        return v;
    }

    void alloc::free_value(value& val) {
        FTL_ERROR_ON(val.is_dead(), "double free value");

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
            if (reg.owner != NULL && reg.dirty)
                count++;
        return count;
    }

    size_t alloc::count_active_regs() const {
        size_t count = 0;
        for (auto reg : m_regmap)
            if (reg.owner != NULL)
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

    void alloc::prologue() {
        for (reg r : callee_saved_regs)
            m_emitter.push(r);

        // Our stack pointer should be 16 byte aligned to allow us calling
        // functions from generated code. So far, our stack frame has 8 bytes
        // (return address) + register spill (8 registers = 64 bytes) + local
        // storage (64 * 8 = 512 bytes). So we need an extra 8 bytes dummy
        // stack storage to reach alignment.
        i32 frame_size = 64 * sizeof(u64) + 8;
        m_emitter.subi(64, STACK_POINTER, frame_size);

        if (m_base)
            m_emitter.movi(64, BASE_REGISTER, m_base);

        m_emitter.jmpr(RDI);
    }

    void alloc::epilogue() {
        i32 frame_size = 64 * sizeof(u64) + 8;
        m_emitter.addi(64, STACK_POINTER, frame_size);

        for (size_t i = FTL_ARRAY_SIZE(callee_saved_regs); i != 0; i--)
            m_emitter.pop(callee_saved_regs[i-1]);

        m_emitter.ret();
    }

    void alloc::reset() {
        for (auto val : m_values)
            val->mark_dead();

        m_values.clear();
        m_locals = ~0ull;
        m_usecnt = 0;

        for (auto r : all_regs) {
           m_regmap[r].regid = r;
           m_regmap[r].owner = NULL;
           m_regmap[r].dirty = false;
           m_regmap[r].count = 0;
       }
    }

}
