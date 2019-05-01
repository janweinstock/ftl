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

    void alloc::check(value& val) const {
        switch (val.type) {
        case VAL_REG:
        case VAL_DIRTY:
            FTL_ERROR_ON(val.r == NREGS, "unassigned register value");
            FTL_ERROR_ON(m_regs[val.r] != &val, "corrupt register cache");
            break;

        case VAL_MEMORY:
            FTL_ERROR_ON(val.r != NREGS, "memory value in register");
            for (auto v : m_regs)
                FTL_ERROR_ON(v == &val, "corrupt register cache");
            break;

        case VAL_DEAD:
            FTL_ERROR("attempt to operate on dead value");
            break;

        default:
            FTL_ERROR("invalid value type %d", val.type);
        }
    }

    alloc::alloc(emitter& e):
        m_emitter(e),
        m_regs(),
        m_locals(~0),
        m_base(0) {
        // nothing to do
    }

    alloc::~alloc() {
        // nothing to do
    }

    value alloc::new_local(int bits, i64 val) {
        int idx = ffs(m_locals) - 1;
        FTL_ERROR_ON(idx < 0, "out of stack frame memory");
        m_locals &= ~(1 << idx);

        reg r = select();
        value v(bits, r, STACK_POINTER, -idx * sizeof(u64));
        m_emitter.movi(v.bits, v.r, val);
        m_regs[r] = &v; // evil

        return v;
    }

    value alloc::new_global(int bits, u64 addr) {
        if (m_base == 0) {
            m_base = FTL_PAGE_ROUND(addr + FTL_PAGE_SIZE);
            m_emitter.movi(64, BASE_REGISTER, m_base);
        }

        i64 offset = addr - m_base;
        value v(bits, BASE_REGISTER, offset, addr, fits_i32(offset));

        return v;
    }

    void alloc::free_value(value& val) {
        FTL_ERROR_ON(val.is_dead(), "double free value");

        if (is_local(val)) {
            int idx = -val.m.offset / sizeof(u64);
            FTL_ERROR_ON(idx < 0 || idx > 64, "corrupt stack offset");
            m_locals |= (1 << idx);
        }

        if (val.is_reg())
            m_regs[val.r] = NULL;

        val.type = VAL_DEAD;
    }

    static const reg alloc_order[] = {
        RAX, RCX, RDX, RBX, RSI, RDI, R8, R9, R10, R11, R12, R13, R14, R15
    };

    reg alloc::select() {
        // try unused registers first
        for (reg r : alloc_order)
            if (is_empty(r))
                return r;

        // next, try registers that do not need to be flushed
        for (reg r : alloc_order) {
            if (!is_dirty(r)) {
                free(r);
                return r;
            }
        }

        // nothing good found
        reg r = alloc_order[0];
        free(r);
        return r;
    }

    void alloc::free(reg r) {
        if (is_empty(r))
            return;

        FTL_ERROR_ON(is_dirty(r), "attempt to free a dirty register");

        m_regs[r]->r = NREGS;
        m_regs[r]->type = VAL_MEMORY;
        m_regs[r] = NULL;
    }

    void alloc::fetch(value& val, reg r) {
        check(val);

        if (r == NREGS)
            r = val.is_reg() ? val.r : select();

        if (val.is_reg()) { // copy value into r
            if (val.r == r)
                return;

            if (is_dirty(r))
                flush(*m_regs[r]);

            m_emitter.movr(val.bits, r, val.r);
            m_regs[val.r] = NULL;
            m_regs[r] = &val;
            val.r = r;

        } else { // copy value from memory into r

            if (!val.is_reachable())
                m_emitter.movi(64, val.m.r, val.base);
            m_emitter.movr(val.bits, r, val.m);
            if (!val.is_reachable())
                m_emitter.movi(64, val.m.r, m_base);

            val.r = r;
            val.type = VAL_REG;
            m_regs[r] = &val;
        }
    }

    void alloc::store(value& val) {
        check(val);

        if (!val.is_dirty())
            return;

        if (!val.is_reachable())
            m_emitter.movi(64, val.m.r, val.base);
        m_emitter.movr(val.bits, val.m, val.r);
        if (!val.is_reachable())
            m_emitter.movi(64, val.m.r, m_base);

        val.type = VAL_REG;
    }

    void alloc::flush(value& val) {
        check(val);

        if (val.is_mem())
            return;

        if (val.is_dirty())
            store(val);

        free(val.r);
    }

    static const reg callee_saved_regs[] = {
          RBX, RBP, RSI, RDI, R12, R13, R14, R15,
    };

    void alloc::prologue() {
        for (reg r : callee_saved_regs)
            m_emitter.push(r);

        i32 frame_size = 64 * sizeof(u64);
        m_emitter.subi(64, STACK_POINTER, frame_size);

        if (m_base)
            m_emitter.movi(64, BASE_REGISTER, m_base);
    }

    void alloc::epilogue() {
        i32 frame_size = 64 * sizeof(u64);
        m_emitter.addi(64, STACK_POINTER, frame_size);

        for (int i = ARRAY_SIZE(callee_saved_regs); i > 0; i--)
            m_emitter.pop(callee_saved_regs[i-1]);
    }

}
