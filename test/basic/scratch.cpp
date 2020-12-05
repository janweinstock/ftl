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

TEST(scratch, simple) {
    func code("simple");

    value temp = code.gen_scratch_i64("temp", 40);
    code.gen_add(temp, 2);
    code.gen_ret(temp);
    code.finish();

    EXPECT_TRUE(temp.is_dead());

    i64 ret = code();
    EXPECT_EQ(ret, 42);
}

TEST(scratch, jumps) {
    func code("simple");

    value temp = code.gen_scratch_i64("temp");
    label dest = code.gen_label("dest");

    reg r = temp.r();
    EXPECT_TRUE(reg_valid(r)) << "illegal register assigned to scratch value";
    EXPECT_FALSE(temp.is_dead()) << "scratch value inactive after constructor";

    u8* ptr = code.get_cbuffer().get_code_ptr();
    code.gen_jmp(dest); // opcode 0xeb + 8bits offset = 2 bytes

    EXPECT_TRUE(temp.is_dead()) << "scratch value still active after jump";
    EXPECT_TRUE(code.get_alloc().is_empty(r)) << "scratch register not freed";

    size_t nbytes = code.get_cbuffer().get_code_ptr() - ptr;
    EXPECT_EQ(nbytes, 2) << "flush instructions might have been emitted";

    value other = code.gen_scratch_i64("other");

    r = other.r();
    EXPECT_TRUE(reg_valid(r)) << "illegal register assigned to scratch value";
    EXPECT_FALSE(other.is_dead()) << "scratch val inactive after constructor";

    ptr = code.get_cbuffer().get_code_ptr();
    dest.place(true); // force flush

    EXPECT_TRUE(other.is_dead()) << "scratch value still active after label";
    EXPECT_TRUE(code.get_alloc().is_empty(r)) << "scratch register not freed";

    nbytes = code.get_cbuffer().get_code_ptr() - ptr;
    EXPECT_EQ(nbytes, 0) << "flush instructions might have been emitted";
}
