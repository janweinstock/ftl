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

TEST(code, simple) {
    int val = 40;

    code cgen(4 * KiB);
    func* fn = cgen.gen_function();
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

TEST(code, jump) {
    int a = 40;
    int b = 42;

    code cgen(4 * KiB);
    label less(cgen.get_cache());
    func* maxfn = cgen.gen_function();

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

extern "C" int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
