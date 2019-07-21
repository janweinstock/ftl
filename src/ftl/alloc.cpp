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

    void alloc::validate(const value& val) const {
        // Make sure the register value has been assigned to a register and
        // that it also has been registered to this register in our map.
        if (val.is_reg()) {
            FTL_ERROR_ON(val.r == NREGS, "unassigned register value");
            FTL_ERROR_ON(m_regmap[val.r] != &val, "corrupt register map");
        }

        // Make sure the memory value has not been assigned to a register and
        // that our register map has no stall references to it.
        if (val.is_mem()) {
            FTL_ERROR_ON(val.r != NREGS, "memory value in register");
            for (auto v : m_regmap)
                FTL_ERROR_ON(v == &val, "corrupt register map");
        }

        if (val.is_dead())
            FTL_ERROR("attempt to operate on dead value");
    }

    alloc::alloc(emitter& e):
        m_emitter(e),
        m_regmap(),
        m_reguse(),
        m_usecnt(0),
        m_locals(~0ull),
        m_base(0) {
        memset(m_regmap, 0, sizeof(m_regmap));
        memset(m_reguse, 0, sizeof(m_reguse));
    }

    alloc::~alloc() {
        // nothing to do
    }

    void alloc::register_value(value* val) {
        if (stl_contains(m_values, val))
            FTL_ERROR("attempt to register value %p twice", val);
        m_values.push_back(val);
    }

    void alloc::unregister_value(value* val) {
        if (!stl_contains(m_values, val))
            FTL_ERROR("attempt to unregister unknown value %p", val);
        stl_remove_erase(m_values, val);
    }

    value alloc::new_local(const string& name, int bits, i64 val, reg r) {
        value v = new_local_noinit(name, bits, r);
        m_emitter.movi(bits, v.r, val);
        v.mark_dirty();
        return v;
    }

    value alloc::new_local_noinit(const string& name, int bits, reg r) {
        int idx = ffs(m_locals) - 1;
        FTL_ERROR_ON(idx < 0, "out of stack frame memory");
        m_locals &= ~(1 << idx);

        if (r == NREGS)
            r = select();
        else
            flush(r);

        value v(name, bits, *this, r, STACK_POINTER, -idx * sizeof(u64));
        m_regmap[r] = &v;

        return v;
    }

    value alloc::new_global(const string& name, int bits, u64 addr) {
        if (m_base == 0) {
            m_base = FTL_PAGE_ROUND(addr + FTL_PAGE_SIZE);
            m_emitter.movi(64, BASE_REGISTER, m_base);
        }

        i64 offset = addr - m_base;
        bool fits = fits_i32(offset);
        value v(name, bits, *this, BASE_REGISTER, offset, addr, fits);

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
            m_regmap[val.r] = NULL;

        val.mark_dead();
    }

    // According to SYSV/Linux calling convention
    static const reg callee_saved_regs[] = {
        RBP, RBX, RSI, RDI, R12, R13, R14, R15,
    };

    static const reg caller_saved_regs[] = {
        RAX, RCX, RDX, RSP, RDI, RSI, R8, R9, R10, R11
    };

    // Never allocate RSP or RBP, since we need them for addressing local and
    // memory variables. Allocate RAX last, since we need it to return values
    // from function calls. RDX gets spoiled in MUL/DIV operations. Prefer
    // registers which are callee saved and not used for function arguments.
    static const reg alloc_order[] = {
        RBP, R12, R13, R14, R15, R8, R9, R10, R11, RCX, RDX, RSI, RDI, RAX,
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

        // pick least recently used
        reg lru = RAX;
        u64 min = m_reguse[lru];
        for (reg r : alloc_order) {
            if (m_reguse[r] < min) {
                lru = r;
                min = m_reguse[r];
            }
        }

        flush(lru);
        return lru;
    }

    void alloc::assign(reg r, value* val) {
        if (val == NULL) {
            m_regmap[r] = NULL;
            return;
        }

        if (val->r < NREGS)
            m_regmap[val->r] = NULL;

        flush(r);
        val->update_register(r);

        if (r < NREGS) {
            m_regmap[r] = val;
            use(r);
        }
    }

    void alloc::free(reg r) {
        if (is_empty(r))
            return;

        FTL_ERROR_ON(is_dirty(r), "attempt to free a dirty register");

        m_regmap[r]->update_register(NREGS);
        m_regmap[r] = NULL;
    }

    void alloc::use(reg r) {
        FTL_ERROR_ON(r == NREGS, "invalid register use");
        m_reguse[r] = m_usecnt++;
    }

    void alloc::store(reg r) {
        value* val = m_regmap[r];
        if (val != NULL)
            store(*val);
    }

    void alloc::flush(reg r) {
        value* val = m_regmap[r];
        if (val != NULL)
            flush(*val);
    }

    void alloc::store_volatile_regs() {
        for (reg r : caller_saved_regs)
            store(r);
    }

    void alloc::flush_volatile_regs() {
        for (reg r : caller_saved_regs)
            flush(r);
    }

    void alloc::fetch(value& val, reg r) {
        validate(val);

        if (r == NREGS)
            r = val.is_reg() ? val.r : select();

        if (val.is_reg()) { // copy value into r
            if (val.r == r)
                return;

            if (is_dirty(r))
                flush(*m_regmap[r]);

            m_emitter.movr(val.bits, r, val.r);

        } else { // copy value from memory into r

            if (!val.is_reachable())
                m_emitter.movi(64, val.m.r, val.base);
            m_emitter.movr(val.bits, r, val.m);
            if (!val.is_reachable())
                m_emitter.movi(64, val.m.r, m_base);
        }

        assign(r, &val);
    }

    void alloc::store(value& val) {
        validate(val);

        if (!val.is_dirty())
            return;

        if (!val.is_reachable())
            m_emitter.movi(64, val.m.r, val.base);
        m_emitter.movr(val.bits, val.m, val.r);
        if (!val.is_reachable())
            m_emitter.movi(64, val.m.r, m_base);

        val.update_register(val.r);
    }

    void alloc::flush(value& val) {
        validate(val);

        if (val.is_mem())
            return;

        if (val.is_dirty())
            store(val);

        free(val.r);
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
        for (auto& use : m_reguse)
            use = 0;

        for (auto& val : m_regmap)
            val = NULL;
    }

}
