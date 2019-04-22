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

#include "ftl/reg.h"

namespace ftl {

    const char* reg_name(reg r) {
        if (r > REG_R15)
            return "invalid";

        static const char* names[] = {
            "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi",
            "r8",  "r9",  "r10", "r11", "r12", "r13", "r14", "r15"
        };

        return names[r];
    }

}

std::ostream& operator << (std::ostream& os, const ftl::reg& r) {
    os << ftl::reg_name(r);
    return os;
}

std::ostream& operator << (std::ostream& os, const ftl::rm& rm) {
    if (!rm.is_mem)
        return os << rm.r;

    os << "[" << rm.r;
    if (rm.offset) {
        if (rm.offset > 0)
            os << "+";
        os << rm.offset;
    }
    os << "]";
    return os;
}

