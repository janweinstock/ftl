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

#define MAKE_TEST(op, cop, bits, reg, op1, op2)                               \
    TEST(emitter, op ## bits ## _ ## reg ## _ ## op1 ## _ ## op2) {           \
        ftl::i64 val1 = (ftl::i64)(op1);                                      \
        ftl::i##bits val2 = (ftl::i##bits)(ftl::i32)(op2);                    \
        ftl::cache code(1 * ftl::KiB);                                        \
        ftl::emitter emitter(code);                                           \
        entry_func* fn = (entry_func*)code.get_code_ptr();                    \
        emitter.movi(ftl::reg, val1);                                         \
        emitter.op(bits, ftl::reg, val2);                                     \
        emitter.mov(ftl::REG_RAX, ftl::reg);                                  \
        emitter.ret();                                                        \
        ftl::i##bits res = (ftl::i##bits)fn();                                \
        ftl::i##bits ref = (ftl::i##bits)(val1 cop val2);                     \
        EXPECT_EQ(res, ref);                                                  \
    }

// repeat the test set using various parameters (large, negative, zero...)
#define MAKE_TEST_ARGSET(op, cop, bits, reg)                                  \
    MAKE_TEST(op, cop, bits, reg, 0x000000001, 0x00000000)                    \
    MAKE_TEST(op, cop, bits, reg, 0x000000000, 0x00000001)                    \
    MAKE_TEST(op, cop, bits, reg, 0x012345678, 0xfedcba98)                    \
    MAKE_TEST(op, cop, bits, reg, 0x0000101ff, 0x000011ff)


// repeat the test set using various (volatile / caller-saved!) registers
#define MAKE_TEST_REGSET(op, cop, bits)                                       \
    MAKE_TEST_ARGSET(op, cop, bits, REG_RAX)                                  \
    MAKE_TEST_ARGSET(op, cop, bits, REG_RCX)                                  \
    MAKE_TEST_ARGSET(op, cop, bits, REG_R10)                                  \
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
