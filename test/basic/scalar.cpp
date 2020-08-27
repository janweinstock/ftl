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

#include <gtest/gtest.h>

#include "ftl.h"

using namespace ftl;

#ifndef __COUNTER__
#define __COUNTER__ __LINE__
#endif

#define _CONCAT(x, y) x##y
#define CONCAT(x, y)  _CONCAT(x,y)
#define UNIQUE(name)  CONCAT(name, __COUNTER__)

#define EXPECT_EQ32(a, b) EXPECT_FLOAT_EQ(a, b)
#define EXPECT_EQ64(a, b) EXPECT_DOUBLE_EQ(a, b)

#define _MKTEST(op, bits1, bits2, a, b, c, d)                                 \
    TEST(scalar, UNIQUE(op ## _ ## bits1 ## _ ## bits2 ## _)) {               \
        f ## bits1 x = a;                                                     \
        f ## bits2 y = b;                                                     \
        func fn(#op);                                                         \
        scalar _x = fn.gen_global_fp("x", bits1, &x);                         \
        scalar _y = fn.gen_global_fp("y", bits2, &y);                         \
        scalar _z = fn.gen_scratch_fp("z", bits1);                            \
        fn.gen_mov(_z, _x);                                                   \
        fn.gen_ ## op (_x, _y);                                               \
        fn.gen_ ## op (_y, _z);                                               \
        fn.gen_ret();                                                         \
        fn();                                                                 \
        EXPECT_EQ ## bits1 (x, c);                                            \
        EXPECT_EQ ## bits2 (y, d);                                            \
    }

#define MKTEST(op, a, b, c, d)                                                \
        _MKTEST(op, 64, 64, a,    b,    c,    d   )                           \
        _MKTEST(op, 64, 32, a,    b##f, c,    d##f)                           \
        _MKTEST(op, 32, 64, a##f,    b, c##f, d   )                           \
        _MKTEST(op, 32, 32, a##f, b##f, c##f, d##f)

MKTEST(add,  4.0, 2.0, 6.0,  6.0)
MKTEST(sub,  4.0, 2.0, 2.0, -2.0)
MKTEST(mul,  4.0, 2.0, 8.0,  8.0)
MKTEST(div,  4.0, 2.0, 2.0,  0.5)
MKTEST(max,  4.0, 2.0, 4.0,  4.0)
MKTEST(min,  4.0, 2.0, 2.0,  2.0)
MKTEST(sqrt, 9.0, 4.0, 2.0,  3.0)
