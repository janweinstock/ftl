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

#define MKTEST(op, op1, op2, cop)                                             \
    TEST(cmov, op ## _ ## op1 ## _ ## op2) {                                  \
        func code("test");                                                    \
        int res = ~0;                                                         \
        value r = code.gen_global_i32("r", &res);                             \
        value a = code.gen_local_i32("a", op1);                               \
        value b = code.gen_local_i32("b", op2);                               \
        code.gen_cmp(a, b);                                                   \
        code.gen_##op(r, b);                                                  \
        r.flush();                                                            \
        code.gen_ret();                                                       \
        code();                                                               \
        EXPECT_EQ(res, (op1 cop op2) ? op2 : ~0);                             \
    }

MKTEST(cmove,  15, 15, ==)
MKTEST(cmove,  15, 27, ==)
MKTEST(cmove,  27, 15, ==)

MKTEST(cmovne, 15, 15, !=)
MKTEST(cmovne, 15, 27, !=)
MKTEST(cmovne, 27, 15, !=)

MKTEST(cmovl, 15, 15, <)
MKTEST(cmovl, 15, 27, <)
MKTEST(cmovl, 27, 15, <)

MKTEST(cmovle, 15, 15, <=)
MKTEST(cmovle, 15, 27, <=)
MKTEST(cmovle, 27, 15, <=)

MKTEST(cmovg, 15, 15, >)
MKTEST(cmovg, 15, 27, >)
MKTEST(cmovg, 27, 15, >)

MKTEST(cmovge, 15, 15, >=)
MKTEST(cmovge, 15, 27, >=)
MKTEST(cmovge, 27, 15, >=)

extern "C" int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
