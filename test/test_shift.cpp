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

#define MAKE_TEST(op, cop, bits, reg, val, shift)                             \
    TEST(immopsmem, op ## shift ## _ ## bits ## _ ## reg) {                   \
        typedef u##bits (entry_func)(void);                                   \
        u##bits op = (u##bits)val;                                            \
        ftl::cbuf code(1 * ftl::KiB);                                         \
        ftl::emitter emitter(code);                                           \
        entry_func* fn = (entry_func*)code.get_code_ptr();                    \
        emitter.movi(64, reg, (i64)op);                                       \
        emitter.op(bits, reg, shift);                                         \
        emitter.movr(bits, RAX, reg);                                         \
        emitter.ret();                                                        \
        u##bits res = fn();                                                   \
        u##bits ref = (u##bits)((op cop shift));                              \
        EXPECT_EQ(res, ref);                                                  \
    }

#define MAKE_TEST_BITSET(op, cop, reg, val, shift)                            \
        MAKE_TEST(op, cop,  8, reg, val, shift)                               \
        MAKE_TEST(op, cop, 16, reg, val, shift)                               \
        MAKE_TEST(op, cop, 32, reg, val, shift)                               \
        MAKE_TEST(op, cop, 64, reg, val, shift)

#define MAKE_TEST_SET(op, cop)                                                \
        MAKE_TEST_BITSET(op, cop, RCX, 0xaa12aa34ull,  1)                     \
        MAKE_TEST_BITSET(op, cop, RDX, 0xaa12aa34ull,  2)                     \
        MAKE_TEST_BITSET(op, cop, R8,  0xaa12aa34ull,  7)                     \
        MAKE_TEST_BITSET(op, cop, R9,  0xaa12aa34ull, 12)                     \

MAKE_TEST_SET(shli, <<)
MAKE_TEST_SET(shri, >>)

extern "C" int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
