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

    enum val_type {
        VAL_REG    = 0, // value in register and memory
        VAL_DIRTY  = 1, // value in register differs from memory
        VAL_MEMORY = 2, // value only held in memory
        VAL_DEAD   = 3, // value marked end-of-life
    };

    struct value {
        int      bits;
        val_type type;
        u64      base;

        reg      r;
        rm       m;

        bool is_dead()  const { return type == VAL_DEAD; }
        bool is_mem()   const { return type == VAL_MEMORY; }
        bool is_reg()   const { return type == VAL_REG || type == VAL_DIRTY; }
        bool is_dirty() const { return type == VAL_DIRTY; }

        void set_dirty();

        bool is_reachable() const { return base == 0; }

        value(int bits, reg r, reg base, i32 offset);
        value(int bits, reg base, i32 offset, u64 addr, bool fits);

        operator const rm() const;
    };

    inline value::value(int w, reg x, reg breg, i32 offset):
        bits(w), type(VAL_REG), base(0), r(x), m(breg, offset) {
    }

    inline value::value(int w, reg b, i32 off, u64 addr, bool fits):
        bits(w), type(VAL_MEMORY), base(fits ? 0 : addr), r(NREGS), m(b, off) {
    }

    inline value::operator const rm() const {
        FTL_ERROR_ON(is_dead(), "operation on dead value");
        return is_reg() ? r : m;
    }

    inline void value::set_dirty() {
        FTL_ERROR_ON(is_dead(), "operation on dead value");
        if (type == VAL_REG)
            type = VAL_DIRTY;
    }

}

#endif
