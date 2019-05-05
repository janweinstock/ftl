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

TEST(cgen, simple) {
    int val = 40;

    cgen cgen(4 * KiB);
    func fn = cgen.gen_function();
    value a = cgen.gen_local_i32(2);
    value b = cgen.gen_global_i32(&val);
    value c = cgen.gen_local_i32(-2);
    cgen.gen_add(a, b);
    cgen.gen_sub(b, c);
    cgen.gen_ret(a);

    int ret = fn();

    EXPECT_EQ(ret, 42);
    EXPECT_EQ(val, 42);
}

TEST(cgen, jump) {
    int a = 40;
    int b = 42;

    cgen cgen(4 * KiB);
    label less(cgen.get_buffer());
    func maxfn = cgen.gen_function();

    value va = cgen.gen_global_i32(&a);
    value vb = cgen.gen_global_i32(&b);
    cgen.gen_cmp(va, vb);
    cgen.gen_jl(less);
    cgen.gen_ret(va);
    less.place();
    cgen.gen_ret(vb);

    int ret = maxfn();
    int ref = max(a, b);

    EXPECT_EQ(ret, ref);
}

TEST(cgen, func) {
    int a = 22;
    int b = 20;
    int c = 17;
    int d = 11;

    cgen cgen(4 * KiB);

    func addfn(cgen.get_buffer());
    value va = cgen.gen_local_i32(a);
    value vb = cgen.gen_local_i32(b);
    cgen.gen_add(va, vb);
    cgen.gen_ret(va);

    func subfn(cgen.get_buffer());
    value vc = cgen.gen_local_i32(c);
    value vd = cgen.gen_local_i32(d);
    cgen.gen_sub(vc, vd);
    cgen.gen_ret(vc);

    int add = addfn();
    int sub = subfn();

    EXPECT_EQ(add, a + b);
    EXPECT_EQ(sub, c - d);
}

extern "C" int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
