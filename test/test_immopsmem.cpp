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

typedef int (entry_func)(void);

#define MAKE_TEST(op, cop, bits, reg, offset, op1, op2)                       \
    TEST(immopsmem, op ## bits ## _ ## reg ## _ ## op1) {                     \
        ftl::i##bits orig = (ftl::i##bits)(op1);                              \
        ftl::i##bits val1 = (ftl::i##bits)(op1);                              \
        ftl::i##bits val2 = (ftl::i##bits)(ftl::i32)(op2);                    \
        ftl::u64 addr = (ftl::u64)&val1 - offset;                             \
        ftl::cache code(1 * ftl::KiB);                                        \
        ftl::emitter emitter(code);                                           \
        entry_func* fn = (entry_func*)code.get_code_ptr();                    \
        emitter.movi(64, ftl::reg, addr);                                     \
        emitter.op(bits, ftl::memop(ftl::reg, offset), val2);                 \
        emitter.ret();                                                        \
        fn();                                                                 \
        ftl::i##bits ref = (ftl::i##bits)(orig cop val2);                     \
        EXPECT_EQ(val1, ref);                                                 \
    }

// repeat the test set using various parameters (large, negative, zero...)
#define MAKE_TEST_ARGSET(op, cop, bits, reg)                                  \
    MAKE_TEST(op, cop, bits, reg, 0, 1, 1)                                    \
    MAKE_TEST(op, cop, bits, reg, 127, 0xffff1111, 0x0)                       \
    MAKE_TEST(op, cop, bits, reg, 0xffffff, 55, -1024)                        \
    MAKE_TEST(op, cop, bits, reg, 0, 0xfedcba98, 0x12345678)

// repeat the test set using various (volatile / caller-saved!) base registers
#define MAKE_TEST_REGSET(op, cop, bits)                                       \
    MAKE_TEST_ARGSET(op, cop, bits, REG_RAX)                                  \
    MAKE_TEST_ARGSET(op, cop, bits, REG_RDX)                                  \
    MAKE_TEST_ARGSET(op, cop, bits, REG_R9 )                                  \
    MAKE_TEST_ARGSET(op, cop, bits, REG_R11)

// repeat the test set using all support bit widths
#define MAKE_TEST_SET(op, cop)                                                \
    MAKE_TEST_REGSET(op, cop, 64)                                             \
    MAKE_TEST_REGSET(op, cop, 32)                                             \
    MAKE_TEST_REGSET(op, cop, 16)                                             \
    MAKE_TEST_REGSET(op, cop,  8)

MAKE_TEST_SET(addi, +)
MAKE_TEST_SET(subi, -)
MAKE_TEST_SET(ori,  |)
MAKE_TEST_SET(andi, &)
MAKE_TEST_SET(xori, ^)

// ToDo: test adc, sbb and cmp

extern "C" int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
