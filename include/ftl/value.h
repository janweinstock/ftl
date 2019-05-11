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

        enum val_type {
            VAL_REG    = 0, // value in register and memory
            VAL_DIRTY  = 1, // value in register differs from memory
            VAL_MEMORY = 2, // value only held in memory
            VAL_DEAD   = 3, // value marked end-of-life
        } m_vt;

        // disabled
        value(const value&);
        value& operator = (const value&);

    public:
        const int bits;
        const u64 base;

        reg r;
        rm  m;

        bool is_dead()  const { return m_vt == VAL_DEAD; }
        bool is_mem()   const { return m_vt == VAL_MEMORY; }
        bool is_reg()   const { return m_vt == VAL_REG || m_vt == VAL_DIRTY; }
        bool is_dirty() const { return m_vt == VAL_DIRTY; }

        void mark_dead();
        void mark_dirty();

        void assign(reg r);

        bool is_reachable() const { return base == 0; }

        value(int bits, alloc& a, reg r, reg base, i32 offset);
        value(int bits, alloc& a, reg base, i32 offset, u64 addr, bool fits);
        value(value&& other);
        virtual ~value();

        operator const rm() const;
        value& operator = (value&& other);
    };

    inline void value::mark_dead() {
        FTL_ERROR_ON(is_dead(), "operation on dead value");
        m_vt = VAL_DEAD;
    }

    inline void value::mark_dirty() {
        FTL_ERROR_ON(is_dead(), "operation on dead value");
        if (m_vt == VAL_REG)
            m_vt = VAL_DIRTY;
    }

    inline void value::assign(reg _r) {
        FTL_ERROR_ON(is_dead(), "operation on dead value");
        r = _r;
        m_vt = (r == NREGS) ? VAL_MEMORY : VAL_REG;
    }

}

#endif
