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

#include "ftl/value.h"
#include "ftl/alloc.h"

namespace ftl {

    static bool valid_width(int width) {
        return width == 8 || width == 16 || width == 32 || width == 64;
    }

    reg value::r() const {
        return m_allocator.lookup(this);
    }

    rm value::mem() const {
        if (m_mem.is_addressable())
            return m_mem;

        reg base = m_allocator.select();
        m_allocator.flush(base);
        m_allocator.get_emitter().movi(64, base, addr);
        return memop(base, 0);
    }

    bool value::is_dirty() const {
        reg curr = r();
        if (!reg_valid(curr))
            return false;

        return m_allocator.is_dirty(curr);
    }

    void value::mark_dirty() {
        reg curr = r();
        if (reg_valid(curr))
            m_allocator.mark_dirty(curr);
    }

    bool value::is_local() const {
        return m_mem.r == m_allocator.STACK_POINTER;
    }

    bool value::is_global() const {
        return m_mem.r == m_allocator.BASE_REGISTER;
    }

    bool value::is_scratch() const {
        return m_mem.r == NREGS;
    }

    bool value::is_reg() const {
        return m_allocator.lookup(this) < NREGS;
    }

    bool value::is_mem() const {
        return m_allocator.lookup(this) == NREGS;
    }

    reg value::assign(reg r) {
        return m_allocator.assign(this, r);
    }

    reg value::fetch(reg r) {
        return m_allocator.fetch(this, r);
    }

    void value::store() {
        reg curr = r();
        if (reg_valid(curr))
            m_allocator.store(curr);
    }

    void value::flush() {
        reg curr = r();
        if (reg_valid(curr))
            m_allocator.flush(curr);
    }

    value::value(alloc& al, const string& nm, int bits, bool sign, u64 addr,
                 reg base, i64 offset):
        m_allocator(al),
        m_name(nm),
        m_dead(false),
        m_mem(base, offset),
        bits(bits),
        sign(sign),
        addr(addr) {
        if (!valid_width(bits))
            FTL_ERROR("value '%s' has invalid bit width %d", name(), bits);
        m_allocator.register_value(this);
    }

    value::value(value&& other):
        m_allocator(other.m_allocator),
        m_name(other.m_name),
        m_dead(other.m_dead),
        m_mem(other.m_mem),
        bits(other.bits),
        sign(other.sign),
        addr(other.addr) {
        m_allocator.register_value(this);

        other.mark_dead();
        if (other.is_reg())
            m_allocator.assign(this, other.r());
    }

    value::~value() {
        if (!is_dead())
            m_allocator.free_value(*this);
        m_allocator.unregister_value(this);
    }

    value::operator const rm() const {
        FTL_ERROR_ON(is_dead(), "operation on dead value");

        reg curr = r();
        if (reg_valid(curr))
            return curr;

        FTL_ERROR_ON(is_scratch(), "attempt to get address of scratch value");

        return mem();
    }

}
