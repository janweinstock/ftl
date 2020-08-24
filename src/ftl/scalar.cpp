/******************************************************************************
 *                                                                            *
 * Copyright 220 Jan Henrik Weinstock                                        *
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

#include "ftl/scalar.h"
#include "ftl/alloc.h"

namespace ftl {

    static bool valid_width(int width) {
        return width == 32 || width == 64;
    }

    xmm scalar::r() const {
        return m_allocator.lookup(this);
    }

    rm scalar::mem() const {
        if (m_mem.is_addressable())
            return m_mem;

        reg base = m_allocator.select();
        m_allocator.flush(base);
        m_allocator.get_emitter().movi(64, base, addr);
        return memop(base, 0);
    }

    bool scalar::is_dirty() const {
        xmm curr = r();
        if (!xmm_valid(curr))
            return false;

        return m_allocator.is_dirty(curr);
    }

    void scalar::mark_dirty() {
        xmm curr = r();
        if (xmm_valid(curr))
            m_allocator.mark_dirty(curr);
    }

    bool scalar::is_local() const {
        return m_mem.r == m_allocator.STACK_POINTER;
    }

    bool scalar::is_global() const {
        return m_mem.r == m_allocator.BASE_REGISTER;
    }

    bool scalar::is_scratch() const {
        return m_mem.r == NREGS;
    }

    bool scalar::is_reg() const {
        return m_allocator.lookup(this) < NXMM;
    }

    bool scalar::is_mem() const {
        return m_allocator.lookup(this) == NXMM;
    }

    xmm scalar::assign(xmm r) {
        return m_allocator.assign(this, r);
    }

    xmm scalar::fetch(xmm r) {
        return m_allocator.fetch(this, r);
    }

    void scalar::store() {
        xmm curr = r();
        if (xmm_valid(curr))
            m_allocator.store(curr);
    }

    void scalar::flush() {
        xmm curr = r();
        if (xmm_valid(curr))
            m_allocator.flush(curr);
    }

    scalar::scalar(alloc& al, const string& nm, int bits, u64 addr, reg base,
                   i64 offset):
        m_allocator(al),
        m_name(nm),
        m_dead(false),
        m_mem(base, offset),
        bits(bits),
        addr(addr) {
        if (!valid_width(bits))
            FTL_ERROR("scalar '%s' has invalid bit width %d", name(), bits);
        m_allocator.register_value(this);
    }

    scalar::scalar(scalar&& other):
        m_allocator(other.m_allocator),
        m_name(other.m_name),
        m_dead(other.m_dead),
        m_mem(other.m_mem),
        bits(other.bits),
        addr(other.addr) {
        m_allocator.register_value(this);

        other.mark_dead();
        if (other.is_reg())
            m_allocator.assign(this, other.r());
    }

    scalar::~scalar() {
        if (!is_dead())
            m_allocator.free_scalar(*this);
        m_allocator.unregister_value(this);
    }

    scalar::operator const rm() const {
        FTL_ERROR_ON(is_dead(), "operation on dead scalar");

        xmm curr = r();
        if (xmm_valid(curr))
            return curr;

        FTL_ERROR_ON(is_scratch(), "attempt to get address of scratch scalar");

        return mem();
    }

}
