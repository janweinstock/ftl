/******************************************************************************
 *                                                                            *
 * Copyright 2020 Jan Henrik Weinstock                                        *
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

#ifndef FTL_SCALAR_H
#define FTL_SCALAR_H

#include "ftl/common.h"
#include "ftl/bitops.h"
#include "ftl/error.h"
#include "ftl/reg.h"

namespace ftl {

    class alloc;

    class scalar
    {
    private:
        alloc& m_allocator;
        string m_name;
        bool   m_dead;
        rm     m_mem;

    public:
        int  bits;
        u64  addr;

        xmm r() const;
        rm  mem() const;

        const char* name() const { return m_name.c_str(); }
        i32 offset() const { return m_mem.offset; }

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

        xmm  assign(xmm r = NXMM);
        xmm  fetch(xmm r = NXMM);
        void store();
        void flush();

        scalar(alloc& al, const string& name, int bits, u64 addr, reg base,
               i64 offset);
        scalar(scalar&& other);
        ~scalar();

        bool operator == (const scalar& other) const;
        bool operator != (const scalar& other) const;

        operator const rm() const;

        scalar(const scalar&) = delete;
        scalar& operator = (const scalar&) = delete;
    };

    inline bool scalar::is_dead() const {
        return m_dead || (is_scratch() && r() == NXMM);
    }

    inline bool scalar::is_directly_addressable() const {
        return !is_scratch() && m_mem.is_addressable();
    }

    inline bool scalar::operator == (const scalar& other) const {
        return this == &other; // values are not copyable
    }

    inline bool scalar::operator != (const scalar& other) const {
        return !operator == (other);
    }

}

#endif
