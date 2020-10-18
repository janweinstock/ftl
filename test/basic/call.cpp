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
    func code("test", 4 * KiB);
    code.set_data_ptr(&test1_called);

    value ret = code.gen_call(test1);
    code.gen_ret(ret);

    u64 result = code();

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
    func code("test", 4 * KiB);
    code.set_data_ptr(&test1_called);

    value op1 = code.gen_local_i64("op1", 1, argreg(1));
    value op2 = code.gen_local_i64("op2", 10, argreg(2));
    value op3 = code.gen_local_i64("op3", 100, argreg(3));
    value ret = code.gen_call(test3, op1, op2, op3);
    code.gen_ret(ret);

    i64 result = code();

    EXPECT_EQ(result, 111);
}

i64 test_boolean(void* bptr, bool arg1, bool arg2) {
    EXPECT_TRUE(arg1);
    EXPECT_FALSE(arg2);
    return 0x11;
}

TEST(call, boolean) {
    func code("test", 4 * KiB);
    code.set_data_ptr(&test1_called);

    value ret = code.gen_call(test_boolean, true, false);
    code.gen_ret(ret);

    i64 result = code();

    EXPECT_EQ(result, 0x11);
}

TEST(call, direct) {
    cbuf buffer(4 * KiB);

    func fn1("fn1", buffer);
    fn1.get_emitter().movr(64, RAX, BASE_POINTER);
    fn1.get_emitter().ret();

    func fn2("fn2", buffer);
    ASSERT_TRUE(can_call_directly(buffer.get_code_ptr(), fn1.entry()));

    value ret = fn2.gen_call(fn1.entry());
    fn2.gen_inc(ret);

    fixup fix;
    fn2.get_emitter().call(nullptr, &fix);
    fn2.gen_ret(ret);

    buffer.skip(1024); // get some distance

    func fn3("fn3", buffer);
    fn3.get_emitter().decr(64, RAX);
    fn3.get_emitter().ret();

    ASSERT_TRUE(can_call_directly(fix.code, fn3.entry()));
    patch_call(fix, fn3.entry());

    i64 result = fn2((void*)42);
    EXPECT_EQ(result, 42);
}

i64 test_fp(void* bptr, i64 val1, f32 val2, f64 val3) {
    EXPECT_EQ(val1, 1337);
    EXPECT_FLOAT_EQ(val2, 1.337f);
    EXPECT_DOUBLE_EQ(val3, 13.37);
    return val1;
}

TEST(call, fp) {
    func code("test", 4 * KiB);

    value  op1 = code.gen_local_i64("op1", 1337);
    scalar op2 = code.gen_local_f32("op2", 1.337f);
    scalar op3 = code.gen_local_f64("op3", 13.37);
    value ret = code.gen_call(test_fp, op1, op2, op3);
    code.gen_ret(ret);

    i64 result = code();

    EXPECT_EQ(result, 1337);
}
