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
    static inline bool fits(T1 val) {
        return val == (T1)((T2)val);
    }

    template <typename T>
    static inline bool fits_i8(T val) {
        return fits<T, i8>(val);
    }

    template <typename T>
    static inline bool fits_i16(T val) {
        return fits<T, i16>(val);
    }

    template <typename T>
    static inline bool fits_i32(T val) {
        return fits<T, i32>(val);
    }

    template <typename T>
    static inline bool fits_i64(T val) {
        return fits<T, i64>(val);
    }

    template <typename T>
    static inline bool fits_u8(T val) {
        return fits<T, u8>(val);
    }

    template <typename T>
    static inline bool fits_u16(T val) {
        return fits<T, u16>(val);
    }

    template <typename T>
    static inline bool fits_u32(T val) {
        return fits<T, u32>(val);
    }

    template <typename T>
    static inline bool fits_u64(T val) {
        return fits<T, u64>(val);
    }

    template <bool is_signed>
    struct encode_traits {
        typedef i8  t8;
        typedef i16 t16;
        typedef i32 t32;
        typedef i64 t64;
    };

    template <>
    struct encode_traits<false> {
         typedef u8  t8;
         typedef u16 t16;
         typedef u32 t32;
         typedef u64 t64;
     };

    template <typename T>
    static inline int encode_size(T val) {
        typedef encode_traits<std::is_signed<T>::value> traits;
        if (fits<T, typename traits::t8>(val))  return 8;
        if (fits<T, typename traits::t16>(val)) return 16;
        if (fits<T, typename traits::t32>(val)) return 32;
        if (fits<T, typename traits::t64>(val)) return 64;
        return sizeof(val) * 8;
    }

    static inline bool is_pow2(u64 val) {
        if (val == 0)
            return false;
        return (val & (val - 1)) == 0;
    }

    static inline int log2i(u64 val) {
        return __builtin_ctzl(val); // works only if val is power of two!
    }

}

#endif
