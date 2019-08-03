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

#define MKTEST(name, a, b, op)                                                \
    TEST(setcc, name ## _ ## a ## _ ## b) {                                   \
        ftl::cbuf code(4 * ftl::KiB);                                         \
        ftl::emitter emitter(code);                                           \
        typedef int (test_func)(int, int);                                    \
        test_func* name = (test_func*)code.get_code_ptr();                    \
        emitter.movi(32, RAX, 0);                                             \
        emitter.cmpr(32, argreg(0), argreg(1));                               \
        emitter.name(RAX);                                                    \
        emitter.ret();                                                        \
        EXPECT_EQ(name(a, b), a op b);                                        \
    }

MKTEST(sete,  15, 15, ==)
MKTEST(sete,  15, 27, ==)
MKTEST(sete,  27, 15, ==)

MKTEST(setne, 15, 15, !=)
MKTEST(setne, 15, 27, !=)
MKTEST(setne, 27, 15, !=)

MKTEST(setl, 15, 15, <)
MKTEST(setl, 15, 27, <)
MKTEST(setl, 27, 15, <)

MKTEST(setle, 15, 15, <=)
MKTEST(setle, 15, 27, <=)
MKTEST(setle, 27, 15, <=)

MKTEST(setg, 15, 15, >)
MKTEST(setg, 15, 27, >)
MKTEST(setg, 27, 15, >)

MKTEST(setge, 15, 15, >=)
MKTEST(setge, 15, 27, >=)
MKTEST(setge, 27, 15, >=)

extern "C" int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
