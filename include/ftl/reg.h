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

#ifndef FTL_REG_H
#define FTL_REG_H

#include "ftl/common.h"
#include "ftl/bitops.h"

namespace ftl {

    enum reg {
        REG_RAX = 0,
        REG_RCX = 1,
        REG_RDX = 2,
        REG_RBX = 3,
        REG_RSP = 4,
        REG_RBP = 5,
        REG_RSI = 6,
        REG_RDI = 7,
        REG_R8  = 8,
        REG_R9  = 9,
        REG_R10 = 10,
        REG_R11 = 11,
        REG_R12 = 12,
        REG_R13 = 13,
        REG_R14 = 14,
        REG_R15 = 15,
    };

    const char* reg_name(reg r);

    struct reg_or_mem {
        bool is_mem;
        reg  r;
        i32  offset;
    };

    static inline reg_or_mem regop(reg r) {
        return { false, r, 0 };
    }

    static inline reg_or_mem memop(reg base, i32 offset) {
        return { true, base, offset };
    }

}

std::ostream& operator << (std::ostream& os, const ftl::reg& r);
std::ostream& operator << (std::ostream& os, const ftl::reg_or_mem& rm);

#endif
