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

namespace ftl {

    enum reg {
        FTL_REG_RAX = 0,
        FTL_REG_RCX = 1,
        FTL_REG_RDX = 2,
        FTL_REG_RBX = 3,
        FTL_REG_RSP = 4,
        FTL_REG_RBP = 5,
        FTL_REG_RSI = 6,
        FTL_REG_RDI = 7,
        FTL_REG_R8  = 8,
        FTL_REG_R9  = 9,
        FTL_REG_R10 = 10,
        FTL_REG_R11 = 11,
        FTL_REG_R12 = 12,
        FTL_REG_R13 = 13,
        FTL_REG_R14 = 14,
        FTL_REG_R15 = 15,
    };

    const char* reg_name(reg r);

}

#endif
