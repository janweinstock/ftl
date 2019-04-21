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

#ifndef FTL_BITOPS_H
#define FTL_BITOPS_H

#include "ftl/common.h"

namespace ftl {

    template <typename T1, typename T2>
    static inline bool fits(const T1& val) {
        return val == (T1)((T2)val);
    }

    template <typename T>
    static inline bool fits_i8(const T& val) {
        return fits<T, i8>(val);
    }

    template <typename T>
    static inline bool fits_i16(const T& val) {
        return fits<T, i16>(val);
    }

    template <typename T>
    static inline bool fits_i32(const T& val) {
        return fits<T, i32>(val);
    }

    template <typename T>
    static inline bool fits_i64(const T& val) {
        return fits<T, i64>(val);
    }

    template <typename T>
    static int encode_size(const T& val) {
        if (fits_i8(val))  return 8;
        if (fits_i16(val)) return 16;
        if (fits_i32(val)) return 32;
        if (fits_i64(val)) return 64;
        return sizeof(T);
    }

}

#endif
