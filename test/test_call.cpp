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

bool test1_called = false;

i64 test1(void* bptr) {
    EXPECT_EQ(bptr, &test1_called);

    test1_called = true;
    return 42;
}

TEST(call, test1) {
    cgen code(4 * KiB);
    code.set_base_ptr(&test1_called);

    func test = code.gen_function();
    value ret = code.gen_call(test1);
    code.gen_ret(ret);

    u64 result = test();

    EXPECT_EQ(result, 42);
    EXPECT_TRUE(test1_called);
}

i64 test3(void* bptr, i64 val1, i64 val2, i64 val3) {
    EXPECT_EQ(val1, 1);
    EXPECT_EQ(val2, 10);
    EXPECT_EQ(val3, 100);
    return val1 + val2 + val3;
}

TEST(call, test3) {
    cgen code(4 * KiB);
    code.set_base_ptr(&test1_called);

    func test = code.gen_function();
    value op1 = code.gen_local_i64(1, argreg(1));
    value op2 = code.gen_local_i64(10, argreg(2));
    value op3 = code.gen_local_i64(100, argreg(3));
    value ret = code.gen_call(test3, op1, op2, op3);
    code.gen_ret(ret);

    i64 result = test();

    EXPECT_EQ(result, 111);
}

extern "C" int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
