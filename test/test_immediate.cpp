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

#define MAKE_TEST_IMMOP(op, bits, reg, op1, op2, cop)                         \
    TEST(emitter, op ## bits ## reg ## op1 ## op2) {                          \
        ftl::cache code(1 * ftl::KiB);                                        \
        ftl::emitter emitter(code);                                           \
        entry_func* fn = (entry_func*)code.get_code_ptr();                    \
        emitter.movi(ftl::reg, (op1));                                          \
        emitter.op(bits, ftl::reg, (op2));                                      \
        emitter.mov(ftl::REG_RAX, ftl::reg);                                  \
        emitter.ret();                                                        \
        EXPECT_EQ(fn(), op1 cop op2);                                         \
    }

#define MAKE_TEST_ADDI(bits, reg, op1, op2)                                   \
        MAKE_TEST_IMMOP(addi, bits, reg, op1, op2, +)
#define MAKE_TEST_ORI(bits, reg, op1, op2)                                    \
        MAKE_TEST_IMMOP(ori, bits, reg, op1, op2, |)
#define MAKE_TEST_ANDI(bits, reg, op1, op2)                                   \
        MAKE_TEST_IMMOP(andi, bits, reg, op1, op2, &)
#define MAKE_TEST_SUBI(bits, reg, op1, op2)                                   \
        MAKE_TEST_IMMOP(subi, bits, reg, op1, op2, -)
#define MAKE_TEST_XORI(bits, reg, op1, op2)                                   \
        MAKE_TEST_IMMOP(xori, bits, reg, op1, op2, ^)

MAKE_TEST_ADDI(64, REG_RAX, 0x0000ffff, 0x0000ffff);
MAKE_TEST_ADDI(64, REG_RAX, 0x00000001, 0x00000001);
MAKE_TEST_ADDI(64, REG_R10, 0x0000ffff, 0x00001234);
MAKE_TEST_ADDI(64, REG_R10, 0x00000001, 0x00000002);

MAKE_TEST_ADDI(32, REG_RAX, 0x0000ffff, 0x0000ffff);
MAKE_TEST_ADDI(32, REG_RAX, 0x00000001, 0x00000001);
MAKE_TEST_ADDI(32, REG_R10, 0x0000ffff, 0x00001234);
MAKE_TEST_ADDI(32, REG_R10, 0x00000001, 0x00000002);

MAKE_TEST_ADDI(16, REG_RAX, 0x00000fff, 0x00000fff);
MAKE_TEST_ADDI(16, REG_RAX, 0x00000001, 0x00000001);
MAKE_TEST_ADDI(16, REG_R10, 0x00000fff, 0x00000123);
MAKE_TEST_ADDI(16, REG_R10, 0x00000001, 0x00000002);

MAKE_TEST_ADDI(8, REG_RAX, 0x0000000f, 0x0000000f);
MAKE_TEST_ADDI(8, REG_RAX, 0x00000001, 0x00000001);
MAKE_TEST_ADDI(8, REG_R10, 0x0000000f, 0x0000000f);
MAKE_TEST_ADDI(8, REG_R10, 0x00000001, 0x00000002);

MAKE_TEST_ORI(64, REG_RAX, 0x0000ffff, 0x0000ffff);
MAKE_TEST_ORI(64, REG_RAX, 0x00000001, 0x00000001);
MAKE_TEST_ORI(64, REG_R10, 0x0000ffff, 0x0000ffff);
MAKE_TEST_ORI(64, REG_R10, 0x00000001, 0x00000002);

MAKE_TEST_ORI(32, REG_RAX, 0x0000ffff, 0x0000ffff);
MAKE_TEST_ORI(32, REG_RAX, 0x00000001, 0x00000001);
MAKE_TEST_ORI(32, REG_R10, 0x0000ffff, 0x0000ffff);
MAKE_TEST_ORI(32, REG_R10, 0x00000001, 0x00000001);

MAKE_TEST_ORI(16, REG_RAX, 0x00000fff, 0x00000fff);
MAKE_TEST_ORI(16, REG_RAX, 0x00000001, 0x00000001);
MAKE_TEST_ORI(16, REG_R10, 0x00000fff, 0x00000123);
MAKE_TEST_ORI(16, REG_R10, 0x00000001, 0x00000001);

MAKE_TEST_ORI(8, REG_RAX, 0x0000000f, 0x0000000f);
MAKE_TEST_ORI(8, REG_RAX, 0x00000001, 0x00000001);
MAKE_TEST_ORI(8, REG_R10, 0x0000000f, 0x0000000f);
MAKE_TEST_ORI(8, REG_R10, 0x00000001, 0x00000001);

MAKE_TEST_ANDI(64, REG_RAX, 0x0000ffff, 0x0000ffff);
MAKE_TEST_ANDI(64, REG_RAX, 0x00000001, 0x00000001);
MAKE_TEST_ANDI(64, REG_R10, 0x0000ffff, 0x0000ffff);
MAKE_TEST_ANDI(64, REG_R10, 0x00000001, 0x00000001);

MAKE_TEST_ANDI(32, REG_RAX, 0x0000ffff, 0x0000ffff);
MAKE_TEST_ANDI(32, REG_RAX, 0x00000001, 0x00000001);
MAKE_TEST_ANDI(32, REG_R10, 0x0000ffff, 0x0000ffff);
MAKE_TEST_ANDI(32, REG_R10, 0x00000001, 0x00000001);

MAKE_TEST_ANDI(16, REG_RAX, 0x00000fff, 0x00000fff);
MAKE_TEST_ANDI(16, REG_RAX, 0x00000001, 0x00000001);
MAKE_TEST_ANDI(16, REG_R10, 0x00000fff, 0x00000fff);
MAKE_TEST_ANDI(16, REG_R10, 0x00000001, 0x00000001);

MAKE_TEST_ANDI(8, REG_RAX, 0x0000000f, 0x0000000f);
MAKE_TEST_ANDI(8, REG_RAX, 0x00000001, 0x00000001);
MAKE_TEST_ANDI(8, REG_R10, 0x0000000f, 0x0000000f);
MAKE_TEST_ANDI(8, REG_R10, 0x00000001, 0x00000001);

MAKE_TEST_SUBI(64, REG_RAX, 0x0000ffff, 0x0000ffff);
MAKE_TEST_SUBI(64, REG_RAX, 0x00000001, 0x00000001);
MAKE_TEST_SUBI(64, REG_R10, 0x0000ffff, 0x0000ffff);
MAKE_TEST_SUBI(64, REG_R10, 0x00000001, 0x00000001);

MAKE_TEST_SUBI(32, REG_RAX, 0x0000ffff, 0x0000ffff);
MAKE_TEST_SUBI(32, REG_RAX, 0x00000001, 0x00000001);
MAKE_TEST_SUBI(32, REG_R10, 0x0000ffff, 0x0000ffff);
MAKE_TEST_SUBI(32, REG_R10, 0x00000001, 0x00000001);

MAKE_TEST_SUBI(16, REG_RAX, 0x00000fff, 0x00000fff);
MAKE_TEST_SUBI(16, REG_RAX, 0x00000001, 0x00000001);
MAKE_TEST_SUBI(16, REG_R10, 0x00000fff, 0x00000fff);
MAKE_TEST_SUBI(16, REG_R10, 0x00000001, 0x00000001);

MAKE_TEST_SUBI(8, REG_RAX, 0x0000000f, 0x0000000f);
MAKE_TEST_SUBI(8, REG_RAX, 0x00000001, 0x00000001);
MAKE_TEST_SUBI(8, REG_R10, 0x0000000f, 0x0000000f);
MAKE_TEST_SUBI(8, REG_R10, 0x00000001, 0x00000001);

MAKE_TEST_XORI(64, REG_RAX, 0x0000ffff, 0x0000ffff);
MAKE_TEST_XORI(64, REG_RAX, 0x00000001, 0x00000001);
MAKE_TEST_XORI(64, REG_R10, 0x0000ffff, 0x0000ffff);
MAKE_TEST_XORI(64, REG_R10, 0x00000001, 0x00000001);

MAKE_TEST_XORI(32, REG_RAX, 0x0000ffff, 0x0000ffff);
MAKE_TEST_XORI(32, REG_RAX, 0x00000001, 0x00000001);
MAKE_TEST_XORI(32, REG_R10, 0x0000ffff, 0x0000ffff);
MAKE_TEST_XORI(32, REG_R10, 0x00000001, 0x00000001);

MAKE_TEST_XORI(16, REG_RAX, 0x00000fff, 0x00000fff);
MAKE_TEST_XORI(16, REG_RAX, 0x00000001, 0x00000001);
MAKE_TEST_XORI(16, REG_R10, 0x00000fff, 0x00000fff);
MAKE_TEST_XORI(16, REG_R10, 0x00000001, 0x00000001);

MAKE_TEST_XORI(8, REG_RAX, 0x0000000f, 0x0000000f);
MAKE_TEST_XORI(8, REG_RAX, 0x00000001, 0x00000001);
MAKE_TEST_XORI(8, REG_R10, 0x0000000f, 0x0000000f);
MAKE_TEST_XORI(8, REG_R10, 0x00000001, 0x00000001);

extern "C" int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
