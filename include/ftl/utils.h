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

#ifndef FTL_UTILS_H
#define FTL_UTILS_H

#include "ftl/common.h"

namespace ftl {

    string mkstr(const char* format, ...);
    string vmkstr(const char* format, va_list args);

    template <typename V, typename T>
    inline void stl_remove_erase(V& v, const T& t) {
        v.erase(std::remove(v.begin(), v.end(), t), v.end());
    }

    template <typename V, typename T>
    inline bool stl_contains(const V& v, const T& t) {
        return std::find(v.begin(), v.end(), t) != v.end();
    }

}

#endif
