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

    value::value(const string& nm, int w, alloc& a, reg x, reg breg, i32 off):
        m_name(nm),
        m_allocator(a),
        m_vt(VAL_REG),
        bits(w),
        base(0),
        r(x),
        m(breg, off) {
        if (!valid_width(w))
            FTL_ERROR("value '%s' has invalid bit width %d", name(), w);
        m_allocator.register_value(this);
    }

    value::value(const string& nm, int width, alloc& al, reg base, i32 offset,
                 u64 addr, bool fits):
        m_name(nm),
        m_allocator(al),
        m_vt(VAL_MEMORY),
        bits(width),
        base(fits ? 0 : addr),
        r(NREGS),
        m(base, offset) {
        if (!valid_width(width))
            FTL_ERROR("value '%s' has invalid bit width %d", name(), width);
        m_allocator.register_value(this);
    }

    value::value(value&& other):
        m_name(other.m_name),
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
