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

std::ostream& operator << (std::ostream& os, const ftl::reg& r) {
    if (ftl::reg_valid(r))
        os << ftl::reg_names[r];
    else
        os << "???";
    return os;
}

std::ostream& operator << (std::ostream& os, const ftl::xmm& r) {
    if (ftl::xmm_valid(r))
        os << "xmm" << (int)r;
    else
        os << "???";
    return os;
}

std::ostream& operator << (std::ostream& os, const ftl::rm& rm) {
    if (!rm.is_mem) {
        if (rm.is_xmm) return os << (ftl::xmm)rm.r;
        else           return os << (ftl::reg)rm.r;
    }

    os << "[" << (ftl::reg)rm.r;
    if (rm.offset) {
        if (rm.offset > 0)
            os << "+";
        os << rm.offset;
    }
    os << "]";
    return os;
}

