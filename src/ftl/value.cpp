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

    void value::assign(reg r) {
        m_allocator.assign(r == NREGS ? m_allocator.select() : r, this);
    }

    void value::fetch(reg r) {
        m_allocator.fetch(*this, r);
    }

    void value::store() {
        m_allocator.store(*this);
    }

    void value::flush() {
        m_allocator.flush(*this);
    }

    value::value(int w, alloc& a, reg x, reg breg, i32 offset):
        m_allocator(a),
        m_vt(VAL_REG),
        bits(w),
        base(0),
        r(x),
        m(breg, offset) {
        m_allocator.register_value(this);
    }

    value::value(int w, alloc& a, reg b, i32 off, u64 addr, bool fits):
        m_allocator(a),
        m_vt(VAL_MEMORY),
        bits(w),
        base(fits ? 0 : addr),
        r(NREGS),
        m(b, off) {
        m_allocator.register_value(this);
    }

    value::value(value&& other):
        m_allocator(other.m_allocator),
        m_vt(other.m_vt),
        bits(other.bits),
        base(other.base),
        r(other.r),
        m(other.m) {
        m_allocator.register_value(this);

        if (is_reg())
            m_allocator.assign(r, this);

        other.mark_dead();
    }

    value::~value() {
        if (!is_dead())
            m_allocator.free_value(*this);
        m_allocator.unregister_value(this);
    }

    value::operator const rm() const {
        FTL_ERROR_ON(is_dead(), "operation on dead value");
        if (is_mem())
            return m;

        m_allocator.use(r);
        return r;
    }

    value& value::operator = (value&& other) {
        FTL_ERROR_ON(bits != other.bits, "different value size");
        FTL_ERROR_ON(base != other.base, "different value address");

        m_vt = other.m_vt;
        r = other.r;
        m = other.m;

        if (is_reg())
            m_allocator.assign(r, this);

        other.m_vt = VAL_DEAD;
        return *this;
    };

}
