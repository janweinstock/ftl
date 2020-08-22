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
#include <fenv.h>

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

#define MKTEST_ARITH(name, bits, a, b, expected)                              \
    TEST(fp, UNIQUE(name ## _ ## bits)) {                                     \
        ftl::cbuf code(4 * ftl::KiB);                                         \
        ftl::emitter emitter(code);                                           \
        typedef f ## bits type;                                               \
        typedef type (test_func)(type, type);                                 \
        test_func* name = (test_func*)code.get_code_ptr();                    \
        emitter.name(bits, XMM0, XMM1);                                       \
        emitter.ret();                                                        \
        EXPECT_DOUBLE_EQ(name(a, b), expected);                               \
    }

MKTEST_ARITH(adds, 32, 1.2f, 2.1f, 1.2f + 2.1f);
MKTEST_ARITH(adds, 64, 1.2,  2.1,  1.2  + 2.1 );
MKTEST_ARITH(subs, 32, 1.2f, 2.1f, 1.2f - 2.1f);
MKTEST_ARITH(subs, 64, 1.2,  2.1,  1.2  - 2.1 );
MKTEST_ARITH(muls, 32, 1.5f, 2.1f, 1.5f * 2.1f);
MKTEST_ARITH(muls, 64, 1.5,  2.0,  1.5  * 2.0 );
MKTEST_ARITH(divs, 32, 1.0f, 2.0f, 1.0f / 2.0f);
MKTEST_ARITH(divs, 64, 1.0,  2.0,  1.0  / 2.0 );
MKTEST_ARITH(mins, 32, 1.0f, 2.0f, std::min(1.0f, 2.0f));
MKTEST_ARITH(mins, 64, 1.0,  2.0,  std::min(1.0,  2.0 ));
MKTEST_ARITH(maxs, 32, 1.0f, 2.0f, std::max(1.0f, 2.0f));
MKTEST_ARITH(maxs, 64, 1.0,  2.0,  std::max(1.0,  2.0 ));

TEST(fp, movss) {
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

#define MKTEST_F2I(f_bits, i_bits, val, expected)                             \
    TEST(fp, UNIQUE(convert_f ## f_bits ##  _to_ ## i ## i_bits ## _)) {      \
        ftl::cbuf code(4 * ftl::KiB);                                         \
        ftl::emitter emitter(code);                                           \
        f ## f_bits f = val;                                                  \
        typedef i ## i_bits (test_func)(void);                                \
        test_func* fn = (test_func*)code.get_code_ptr();                      \
        emitter.movi(64, RAX, (i64)&f);                                       \
        emitter.cvts2i(i_bits, f_bits, RAX, memop(RAX, 0));                   \
        emitter.ret();                                                        \
        EXPECT_EQ(fn(), (i ## i_bits) expected);                              \
    }

MKTEST_F2I(32, 32,  0.0000,  0);
MKTEST_F2I(32, 32,  2.0000,  2);
MKTEST_F2I(32, 32, -2.0000, -2);
MKTEST_F2I(32, 32,  3.1415,  3);
MKTEST_F2I(32, 32, -3.1415, -3);
MKTEST_F2I(32, 32,  3.9999,  4);

MKTEST_F2I(32, 64,  0.0000,  0);
MKTEST_F2I(32, 64,  2.0000,  2);
MKTEST_F2I(32, 64, -2.0000, -2);
MKTEST_F2I(32, 64,  3.1415,  3);
MKTEST_F2I(32, 64, -3.1415, -3);
MKTEST_F2I(32, 64,  3.9999,  4);

MKTEST_F2I(64, 32,  0.0000,  0);
MKTEST_F2I(64, 32,  2.0000,  2);
MKTEST_F2I(64, 32, -2.0000, -2);
MKTEST_F2I(64, 32,  3.1415,  3);
MKTEST_F2I(64, 32, -3.1415, -3);
MKTEST_F2I(64, 32,  3.9999,  4);

MKTEST_F2I(64, 64,  0.0000,  0);
MKTEST_F2I(64, 64,  2.0000,  2);
MKTEST_F2I(64, 64, -2.0000, -2);
MKTEST_F2I(64, 64,  3.1415,  3);
MKTEST_F2I(64, 64, -3.1415, -3);
MKTEST_F2I(64, 64,  3.9999,  4);

#define MKTEST_F2I_TRUNC(f_bits, i_bits, val, expected)                       \
    TEST(fp, UNIQUE(convert_f ## f_bits ##  _to_ ## i ## i_bits ## _)) {      \
        ftl::cbuf code(4 * ftl::KiB);                                         \
        ftl::emitter emitter(code);                                           \
        f ## f_bits f = val;                                                  \
        typedef u ## i_bits (test_func)(void);                                \
        test_func* fn = (test_func*)code.get_code_ptr();                      \
        emitter.movi(64, RAX, (i64)&f);                                       \
        emitter.cvtts2i(i_bits, f_bits, RAX, memop(RAX, 0));                  \
        emitter.ret();                                                        \
        EXPECT_EQ(fn(), (u ## i_bits) expected);                              \
    }

MKTEST_F2I_TRUNC(32, 32,  0.0000,  0);
MKTEST_F2I_TRUNC(32, 32,  2.0000,  2);
MKTEST_F2I_TRUNC(32, 32, -2.0000, -2);
MKTEST_F2I_TRUNC(32, 32,  3.1415,  3);
MKTEST_F2I_TRUNC(32, 32, -3.1415, -3);
MKTEST_F2I_TRUNC(32, 32,  3.9999,  3);

MKTEST_F2I_TRUNC(32, 64,  0.0000,  0);
MKTEST_F2I_TRUNC(32, 64,  2.0000,  2);
MKTEST_F2I_TRUNC(32, 64, -2.0000, -2);
MKTEST_F2I_TRUNC(32, 64,  3.1415,  3);
MKTEST_F2I_TRUNC(32, 64, -3.1415, -3);
MKTEST_F2I_TRUNC(32, 64,  3.9999,  3);

MKTEST_F2I_TRUNC(64, 32,  0.0000,  0);
MKTEST_F2I_TRUNC(64, 32,  2.0000,  2);
MKTEST_F2I_TRUNC(64, 32, -2.0000, -2);
MKTEST_F2I_TRUNC(64, 32,  3.1415,  3);
MKTEST_F2I_TRUNC(64, 32, -3.1415, -3);
MKTEST_F2I_TRUNC(64, 32,  3.9999,  3);

MKTEST_F2I_TRUNC(64, 64,  0.0000,  0);
MKTEST_F2I_TRUNC(64, 64,  2.0000,  2);
MKTEST_F2I_TRUNC(64, 64, -2.0000, -2);
MKTEST_F2I_TRUNC(64, 64,  3.1415,  3);
MKTEST_F2I_TRUNC(64, 64, -3.1415, -3);
MKTEST_F2I_TRUNC(64, 64,  3.9999,  3);

#define MKTEST_I2F(f_bits, i_bits, val, expected)                             \
    TEST(fp, UNIQUE(convert_f ## f_bits ##  _to_ ## i ## i_bits ## _)) {      \
        ftl::cbuf code(4 * ftl::KiB);                                         \
        ftl::emitter emitter(code);                                           \
        i ## i_bits i = val;                                                  \
        typedef f ## f_bits (test_func)(void);                                \
        test_func* fn = (test_func*)code.get_code_ptr();                      \
        emitter.movi(64, RAX, (i64)&i);                                       \
        emitter.cvti2s(i_bits, f_bits, XMM0, memop(RAX, 0));                  \
        emitter.ret();                                                        \
        EXPECT_DOUBLE_EQ(fn(), (f ## f_bits) expected);                       \
    }

MKTEST_I2F(32, 32,  0,  0.0);
MKTEST_I2F(32, 32, -1, -1.0);
MKTEST_I2F(32, 32,  2,  2.0);
MKTEST_I2F(32, 32, -2, -2.0);
MKTEST_I2F(32, 32, 99, 99.0);

MKTEST_I2F(32, 64,  0,  0.0);
MKTEST_I2F(32, 64, -1, -1.0);
MKTEST_I2F(32, 64,  2,  2.0);
MKTEST_I2F(32, 64, -2, -2.0);
MKTEST_I2F(32, 64, 99, 99.0);

MKTEST_I2F(64, 32,  0,  0.0);
MKTEST_I2F(64, 32, -1, -1.0);
MKTEST_I2F(64, 32,  2,  2.0);
MKTEST_I2F(64, 32, -2, -2.0);
MKTEST_I2F(64, 32, 99, 99.0);

MKTEST_I2F(64, 64,  0,  0.0);
MKTEST_I2F(64, 64, -1, -1.0);
MKTEST_I2F(64, 64,  2,  2.0);
MKTEST_I2F(64, 64, -2, -2.0);
MKTEST_I2F(64, 64, 99, 99.0);

#define MKTEST_F2F(t_bits, s_bits, val)                                       \
    TEST(fp, UNIQUE(convert_f ## s_bits ##  _to_ ## f ## t_bits ## _)) {      \
        ftl::cbuf code(4 * ftl::KiB);                                         \
        ftl::emitter emitter(code);                                           \
        f ## s_bits f = val;                                                  \
        typedef f ## t_bits (test_func)(void);                                \
        test_func* fn = (test_func*)code.get_code_ptr();                      \
        emitter.movi(64, RAX, (i64)&f);                                       \
        emitter.cvts2s(t_bits, s_bits, XMM0, memop(RAX, 0));                  \
        emitter.ret();                                                        \
        EXPECT_EQ##t_bits(fn(), (f ## t_bits) val);                           \
    }

MKTEST_F2F(32, 32,  0.0000f);
MKTEST_F2F(32, 32,  2.0000f);
MKTEST_F2F(32, 32, -2.0000f);
MKTEST_F2F(32, 32,  3.1415f);
MKTEST_F2F(32, 32, -3.1415f);
MKTEST_F2F(32, 32,  3.9999f);

MKTEST_F2F(32, 64,  0.0000);
MKTEST_F2F(32, 64,  2.0000);
MKTEST_F2F(32, 64, -2.0000);
MKTEST_F2F(32, 64,  3.1415);
MKTEST_F2F(32, 64, -3.1415);
MKTEST_F2F(32, 64,  3.9999);

MKTEST_F2F(64, 32,  0.0000f);
MKTEST_F2F(64, 32,  2.0000f);
MKTEST_F2F(64, 32, -2.0000f);
MKTEST_F2F(64, 32,  3.1415f);
MKTEST_F2F(64, 32, -3.1415f);
MKTEST_F2F(64, 32,  3.9999f);

MKTEST_F2F(64, 64,  0.0000);
MKTEST_F2F(64, 64,  2.0000);
MKTEST_F2F(64, 64, -2.0000);
MKTEST_F2F(64, 64,  3.1415);
MKTEST_F2F(64, 64, -3.1415);
MKTEST_F2F(64, 64,  3.9999);

#define MKTEST_FCMP(bits, a, b)                                               \
    TEST(fp, UNIQUE(compare ## bits ## _)) {                                  \
        ftl::cbuf code(4 * ftl::KiB);                                         \
        ftl::emitter emitter(code);                                           \
        typedef bool (test_func)(f ## bits, f ## bits);                       \
        test_func* fn = (test_func*)code.get_code_ptr();                      \
        emitter.comis(bits, argxmm(0), argxmm(1));                            \
        emitter.seta(RAX);                                                    \
        emitter.ret();                                                        \
        EXPECT_EQ ## bits(fn(a, b), a > b);                                   \
    }

MKTEST_FCMP(32, -1.0f, 1.0000f)
MKTEST_FCMP(32,  2.0f, 3.1415f)


MKTEST_FCMP(64,  2.0, 3.1415)
MKTEST_FCMP(64, -1.0, 1.0000)
