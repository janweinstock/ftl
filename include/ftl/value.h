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

#ifndef FTL_VALUE_H
#define FTL_VALUE_H

#include "ftl/common.h"
#include "ftl/bitops.h"
#include "ftl/error.h"
#include "ftl/reg.h"

namespace ftl {

    class alloc;

    class value
    {
    private:
        alloc& m_allocator;
        string m_name;
        bool   m_dead;

    public:
        int  bits;
        bool sign;
        u64  addr;

        rm  mem;
        reg r() const;

        const char* name() const { return m_name.c_str(); }

        bool is_dead() const;
        void mark_dead() { m_dead = true; }

        bool is_dirty() const;
        void mark_dirty();

        bool is_local() const;
        bool is_global() const;
        bool is_scratch() const;

        bool is_reg() const;
        bool is_mem() const;

        bool is_directly_addressable() const;

        reg  assign(reg r = NREGS);
        reg  fetch(reg r = NREGS);
        void store();
        void flush();

        value(alloc& al, const string& name, int bits, bool sign, u64 addr,
              reg base, i32 offset);
        value(value&& other);
        ~value();

        bool operator == (const value& other) const;
        bool operator != (const value& other) const;

        operator const rm() const;

        value(const value&) = delete;
        value& operator = (const value&) = delete;
    };

    inline bool value::is_dead() const {
        return m_dead || (is_scratch() && r() == NREGS);
    }

    inline bool value::operator == (const value& other) const {
        return bits == other.bits &&
               sign == other.sign &&
               addr == other.addr &&
               mem  == other.mem;
    }

    inline bool value::operator != (const value& other) const {
        return !operator == (other);
    }

}

#endif
