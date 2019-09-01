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

#ifndef FTL_COMMON_H
#define FTL_COMMON_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <string>
#include <vector>
#include <array>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <type_traits>

#include <unistd.h>
#include <sys/mman.h>

#define FTL_PAGE_SIZE        (4096)
#define FTL_PAGE_MASK(addr)  ((addr) & ~(FTL_PAGE_SIZE - 1))
#define FTL_PAGE_ROUND(addr) (FTL_PAGE_MASK(addr + FTL_PAGE_SIZE - 1))

#define FTL_ARRAY_SIZE(a)    (sizeof(a) / sizeof((a)[0]))

namespace ftl {

    typedef int8_t  i8;
    typedef int16_t i16;
    typedef int32_t i32;
    typedef int64_t i64;

    typedef uint8_t  u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;

    const size_t KiB = 1024;
    const size_t MiB = 1024 * KiB;
    const size_t GiB = 1024 * MiB;
    const size_t TiB = 1024 * GiB;

    using std::min;
    using std::max;

    using std::string;
    using std::vector;
    using std::array;
    using std::stringstream;

}

#endif
