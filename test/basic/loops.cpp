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

i64 append(void* ptr, i64 val) {
    std::stringstream* ss = (std::stringstream*)ptr;
    *ss << val;
    return val;
}

TEST(loops, simple) {
    i64 sum = 0;
    std::stringstream ss;


    func code("fn");
    code.set_data_ptr(&ss);

    value i = code.gen_local_i32("i", 0);
    value s = code.gen_global_i64("sum", &sum);

    label loop = code.gen_label("loop");
    loop.place();

    value r = code.gen_call(append, i);
    code.gen_add(s, r);
    code.free_value(r);

    code.gen_add(i, 1);
    code.gen_cmp(i, 10);
    code.gen_jl(loop);
    code.gen_ret();

    code.free_value(s);
    code.free_value(i);
    code.finish();

    code();

    EXPECT_EQ(ss.str(), "0123456789");
    EXPECT_EQ(sum, 45);
}
