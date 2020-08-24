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
        m_regs(e),
        m_xmms(e),
        m_locals(~0ull),
        m_base(0) {
        reset();
    }

    alloc::~alloc() {
        // nothing to do
    }

    reg alloc::assign(const value* val, reg r) {
        FTL_ERROR_ON(!val, "attempt to assign nullptr value");

        if (r == NREGS)
            r = m_regs.lookup(val);
        if (r == NREGS)
            r = m_regs.select();

        FTL_ERROR_ON(!reg_valid(r), "invalid register selected: %d", r);
        FTL_ERROR_ON(r == STACK_POINTER, "cannot assign to stack pointer");
        FTL_ERROR_ON(r == BASE_REGISTER, "cannot assign to base register");
        FTL_ERROR_ON(is_blocked(r), "cannot assign to blocked register %s",
                     reg_names[r]);

        if (m_regs.lookup(r) == val)
            return r;

        flush(r);

        reg curr = m_regs.lookup(val);
        if (curr < NREGS)
            m_regs.assign(curr, nullptr);

        m_regs.assign(r, val);
        return r;
    }

    xmm alloc::assign(const scalar* val, xmm r) {
        FTL_ERROR_ON(!val, "attempt to assign nullptr scalar");

        if (r == NXMM)
            r = m_xmms.lookup(val);
        if (r == NXMM)
            r = m_xmms.select();

        FTL_ERROR_ON(!reg_valid(r), "invalid register selected: %d", r);
        FTL_ERROR_ON(is_blocked(r), "cannot assign to blocked register %s",
                     xmm_names[r]);

        if (m_xmms.lookup(r) == val)
            return r;

        flush(r);

        xmm curr = m_xmms.lookup(val);
        if (curr < NXMM)
            m_xmms.assign(curr, nullptr);

        m_xmms.assign(r, val);

        return r;
    }

    reg alloc::fetch(const value* val, reg r) {
        FTL_ERROR_ON(!val, "attempt to fetch nullptr value");

        reg curr = m_regs.lookup(val);
        if ((curr < NREGS) && (curr == r || r == NREGS))
            return curr;

        r = assign(val, r);

        if (curr < NREGS) {
            m_emitter.movr(val->bits, r, curr);
        } else {
            FTL_ERROR_ON(val->is_scratch(), "attempt to fetch scratch value");
            m_emitter.movr(val->bits, r, val->mem());
        }

        return r;
    }

    xmm alloc::fetch(const scalar* val, xmm r) {
        FTL_ERROR_ON(!val, "attempt to fetch nullptr value");

        xmm curr = m_xmms.lookup(val);
        if ((curr < NXMM) && (curr == r || r == NXMM))
            return curr;

        r = assign(val, r);

        if (curr < NXMM) {
            m_emitter.movs(val->bits, r, curr);
        } else {
            FTL_ERROR_ON(val->is_scratch(), "attempt to fetch scratch value");
            m_emitter.movs(val->bits, r, val->mem());
        }

        return r;
    }

    void alloc::store(reg r) {
        FTL_ERROR_ON(!reg_valid(r), "invalid register specified");

        if (!is_dirty(r))
            return;

        const value* val = m_regs.lookup(r);
        FTL_ERROR_ON(val == nullptr, "store operation on empty register");

        if (val->is_scratch())
            return;

        m_emitter.movr(val->bits, val->mem(), r);
        mark_clean(r);
    }

    void alloc::store(xmm r) {
        FTL_ERROR_ON(!xmm_valid(r), "invalid register specified");

        if (!is_dirty(r))
            return;

        const scalar* val = m_xmms.lookup(r);
        FTL_ERROR_ON(val == nullptr, "store operation on empty register");

        if (val->is_scratch())
            return;

        m_emitter.movs(val->bits, val->mem(), r);
        mark_clean(r);
    }

    void alloc::flush(reg r) {
        FTL_ERROR_ON(!reg_valid(r), "invalid register specified");
        store(r);
        m_regs.assign(r, nullptr);
    }

    void alloc::flush(xmm r) {
        FTL_ERROR_ON(!xmm_valid(r), "invalid register specified");
        store(r);
        m_xmms.assign(r, nullptr);
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

        i64 offset = addr - m_base;
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
            int idx = -val.offset() / sizeof(u64);
            FTL_ERROR_ON(idx < 0 || idx > 64, "corrupt stack offset");
            m_locals |= (1 << idx);
        }

        reg r = lookup(&val);
        if (r < NREGS)
            m_regs.assign(r, nullptr);

        val.mark_dead();
    }

    void alloc::free_scalar(scalar& val) {
        FTL_ERROR_ON(val.is_dead(), "double free scalar %s", val.name());

        if (val.is_local()) {
            int idx = -val.offset() / sizeof(u64);
            FTL_ERROR_ON(idx < 0 || idx > 64, "corrupt stack offset");
            m_locals |= (1 << idx);
        }

        xmm r = lookup(&val);
        if (r < NXMM)
            m_xmms.assign(r, nullptr);

        val.mark_dead();
    }

    size_t alloc::count_active_regs() const {
        size_t count = 0;
        count += m_regs.count_active_regs();
        count += m_xmms.count_active_regs();
        return count;
    }

    size_t alloc::count_dirty_regs() const {
        size_t count = 0;
        count += m_regs.count_dirty_regs();
        count += m_xmms.count_dirty_regs();
        return count;
    }

    void alloc::store_all_regs() {
        for (reg r : all_regs)
            store(r);
        for (xmm r : all_xmms)
            store(r);
    }

    void alloc::flush_all_regs() {
        for (reg r : all_regs)
            flush(r);
        for (xmm r : all_xmms)
            flush(r);
    }

    void alloc::store_volatile_regs() {
        for (reg r : caller_saved_regs)
            store(r);
        for (xmm r : caller_saved_xmms)
            store(r);
    }

    void alloc::flush_volatile_regs() {
        for (reg r : caller_saved_regs)
            flush(r);
        for (xmm r : caller_saved_xmms)
            flush(r);
    }

    void alloc::reset() {
        m_locals = ~0ull;

        m_regs.reset();
        m_xmms.reset();
    }

}
