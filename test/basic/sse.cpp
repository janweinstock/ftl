/******************************************************************************
 *                                                                            *
 * Copyright 2020 Jan Henrik Weinstock                                        *
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

#define MKTEST(name, type, a, b, expected)                                    \
    TEST(sse, name ## _ ## type) {                                            \
        ftl::cbuf code(4 * ftl::KiB);                                         \
        ftl::emitter emitter(code);                                           \
        typedef type (test_func)(type, type);                                 \
        test_func* name = (test_func*)code.get_code_ptr();                    \
        emitter.name(sizeof(type) * 8, XMM0, XMM1);                           \
        emitter.ret();                                                        \
        EXPECT_DOUBLE_EQ(name(a, b), expected);                               \
    }

MKTEST(adds, float,  1.2f, 2.1f, 1.2f + 2.1f);
MKTEST(adds, double, 1.2,  2.1,  1.2  + 2.1 );
MKTEST(subs, float,  1.2f, 2.1f, 1.2f - 2.1f);
MKTEST(subs, double, 1.2,  2.1,  1.2  - 2.1 );
MKTEST(muls, float,  1.5f, 2.1f, 1.5f * 2.1f);
MKTEST(muls, double, 1.5,  2.0,  1.5  * 2.0 );
MKTEST(divs, float,  1.0f, 2.0f, 1.0f / 2.0f);
MKTEST(divs, double, 1.0,  2.0,  1.0  / 2.0 );
MKTEST(mins, float,  1.0f, 2.0f, std::min(1.0f, 2.0f));
MKTEST(mins, double, 1.0,  2.0,  std::min(1.0,  2.0 ));
MKTEST(maxs, float,  1.0f, 2.0f, std::max(1.0f, 2.0f));
MKTEST(maxs, double, 1.0,  2.0,  std::max(1.0,  2.0 ));

TEST(sse, movss) {
    double a[16], b[16], c[16];
    for (int i = 0; i < 16; i++) {
        a[i] = i;
        b[i] = i * i;
        c[i] = 0;
    }


    cbuf code(1 * KiB);
    emitter emitter(code);

    typedef int (entry_func)(void);
    entry_func* fn1 = (entry_func*)code.get_code_ptr();
    emitter.movi(64, R11, (i64)a);
    emitter.movi(64, R12, (i64)b);
    emitter.movi(64, R13, (i64)c);

    for (int i = 0; i < 16; i++)
        emitter.movs(64, xmm(XMM0 + i), memop(R11, i * 8));

    for (int i = 0; i < 16; i++)
        emitter.adds(64, xmm(XMM0 + i), memop(R12, i * 8));

    for (int i = 0; i < 16; i++)
        emitter.movs(64, memop(R13, i * 8), xmm(XMM0 + i));

    emitter.ret();

    fn1();

    EXPECT_DOUBLE_EQ(a[ 0] + b[ 0], c[ 0]);
    EXPECT_DOUBLE_EQ(a[ 1] + b[ 1], c[ 1]);
    EXPECT_DOUBLE_EQ(a[ 2] + b[ 2], c[ 2]);
    EXPECT_DOUBLE_EQ(a[ 3] + b[ 3], c[ 3]);
    EXPECT_DOUBLE_EQ(a[ 4] + b[ 4], c[ 4]);
    EXPECT_DOUBLE_EQ(a[ 5] + b[ 5], c[ 5]);
    EXPECT_DOUBLE_EQ(a[ 6] + b[ 6], c[ 6]);
    EXPECT_DOUBLE_EQ(a[ 7] + b[ 7], c[ 7]);
    EXPECT_DOUBLE_EQ(a[ 8] + b[ 8], c[ 8]);
    EXPECT_DOUBLE_EQ(a[ 9] + b[ 9], c[ 9]);
    EXPECT_DOUBLE_EQ(a[10] + b[10], c[10]);
    EXPECT_DOUBLE_EQ(a[11] + b[11], c[11]);
    EXPECT_DOUBLE_EQ(a[12] + b[12], c[12]);
    EXPECT_DOUBLE_EQ(a[13] + b[13], c[13]);
    EXPECT_DOUBLE_EQ(a[14] + b[14], c[14]);
    EXPECT_DOUBLE_EQ(a[15] + b[15], c[15]);
}

