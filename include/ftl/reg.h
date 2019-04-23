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
        REG_RAX = 0,  // caller-saved
        REG_RCX = 1,  // caller-saved
        REG_RDX = 2,  // caller-saved
        REG_RBX = 3,
        REG_RSP = 4,
        REG_RBP = 5,
        REG_RSI = 6,
        REG_RDI = 7,
        REG_R8  = 8,  // caller-saved
        REG_R9  = 9,  // caller-saved
        REG_R10 = 10, // caller-saved
        REG_R11 = 11, // caller-saved
        REG_R12 = 12,
        REG_R13 = 13,
        REG_R14 = 14,
        REG_R15 = 15,
    };

    const char* reg_name(reg r);

    struct rm {
        const bool is_mem;
        const reg  r;
        const i32  offset;

        bool is_reg() const { return !is_mem; }

        rm(reg _r): is_mem(false), r(_r), offset(0) {}
        rm(reg base, i32 off): is_mem(true), r(base), offset(off) {}

    private:
        rm(); // disabled
    };

    static inline rm regop(reg r) {
        return rm(r);
    }

    static inline rm memop(reg base, i32 offset) {
        return rm(base, offset);
    }

}

std::ostream& operator << (std::ostream& os, const ftl::reg& r);
std::ostream& operator << (std::ostream& os, const ftl::rm& rm);

#endif
