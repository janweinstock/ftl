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
    func fn = cgen.gen_function("fn");
    value a = cgen.gen_local_i32("a", 2);
    value b = cgen.gen_global_i32("b", &val);
    value c = cgen.gen_local_i32("c", -2);
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
    label less = cgen.gen_label("less");
    func maxfn = cgen.gen_function("maxfn");

    value va = cgen.gen_global_i32("a", &a);
    value vb = cgen.gen_global_i32("b", &b);
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

    func addfn = cgen.gen_function("addfn");
    value va = cgen.gen_local_i32("va", a);
    value vb = cgen.gen_local_i32("vb", b);
    cgen.gen_add(va, vb);
    cgen.gen_ret(va);

    func subfn = cgen.gen_function("subfn");
    value vc = cgen.gen_local_i32("vc", c);
    value vd = cgen.gen_local_i32("vd", d);
    cgen.gen_sub(vc, vd);
    cgen.gen_ret(vc);

    int add = addfn();
    int sub = subfn();

    EXPECT_EQ(add, a + b);
    EXPECT_EQ(sub, c - d);
}

i64 test_muldiv(void* ptr, i64 a, i64 b, i64 c) {
    i32 _a = (i32)a;
    i32 _b = (i32)b;
    i32 _c = (i32)c;
    EXPECT_EQ(_a, 16 * -5);
    EXPECT_EQ(_b, 16 / -5);
    EXPECT_EQ(_c, 16 % -5);
    return 0;
}

TEST(cgen, muldiv) {
    int a = 16;
    int b = -5;

    cgen code(4 * KiB);

    func test_val = code.gen_function("test_val");
    value vb = code.gen_global_i32("vb", &b);
    value vx = code.gen_local_i32("vx", a);
    value vy = code.gen_local_i32("vy", a);
    value vz = code.gen_local_i32("vz", a);
    code.gen_imul(vx, vb);
    code.gen_idiv(vy, vb);
    code.gen_imod(vz, vb);
    code.gen_call(test_muldiv, vx, vy, vz);
    code.gen_ret();

    func test_imm = code.gen_function("test_imm");
    value vx2 = code.gen_local_i32("vx2", a);
    value vy2 = code.gen_local_i32("vy2", a);
    value vz2 = code.gen_local_i32("vz2", a);
    code.gen_imul(vx2, b);
    code.gen_idiv(vy2, b);
    code.gen_imod(vz2, b);
    code.gen_call(test_muldiv, vx2, vy2, vz2);
    code.gen_ret();

    test_val();
    test_imm();
}

i64 test_umuldiv(void* ptr, i64 a, i64 b, i64 c) {
    i32 _a = (i32)a;
    i32 _b = (i32)b;
    i32 _c = (i32)c;
    EXPECT_EQ(_a, 16 * 5);
    EXPECT_EQ(_b, 16 / 5);
    EXPECT_EQ(_c, 16 % 5);
    return 0;
}

TEST(cgen, umuldiv) {
    int a = 16;
    int b = 5;

    cgen code(4 * KiB);

    func test_val = code.gen_function("test_val");
    value vb = code.gen_global_i32("vb", &b);
    value vx = code.gen_local_i32("vx", a);
    value vy = code.gen_local_i32("vy", a);
    value vz = code.gen_local_i32("vz", a);
    code.gen_umul(vx, vb);
    code.gen_udiv(vy, vb);
    code.gen_umod(vz, vb);
    code.gen_call(test_umuldiv, vx, vy, vz);
    code.gen_ret();

    func test_imm = code.gen_function("test_imm");
    value vx2 = code.gen_local_i32("vx2", a);
    value vy2 = code.gen_local_i32("vy2", a);
    value vz2 = code.gen_local_i32("vz2", a);
    code.gen_umul(vx2, b);
    code.gen_udiv(vy2, b);
    code.gen_umod(vz2, b);
    code.gen_call(test_umuldiv, vx2, vy2, vz2);
    code.gen_ret();

    test_val();
    test_imm();
}

i64 test_notneg(void* ptr, i64 a, i64 b) {
    EXPECT_EQ(a, ~42);
    EXPECT_EQ(b, -42);
    return 0;
}

TEST(cgen, notneg) {
    cgen code(4 * KiB);

    func test = code.gen_function("test");
    value va = code.gen_local_i64("va", 42);
    value vb = code.gen_local_i64("vb", 42);
    code.gen_not(va);
    code.gen_neg(vb);
    code.gen_call(test_notneg, va, vb);
    code.gen_ret();

    test();
}

i64 test_shift(void* ptr, i64 a, i64 b, i64 c) {
    EXPECT_EQ(a, 42u << 4);
    EXPECT_EQ(b, 42u >> 2);
    EXPECT_EQ(c, -42 >> 2);
    return 0;
}

TEST(cgen, shift) {
    cgen code(4 * KiB);

    func test = code.gen_function("test");
    value a = code.gen_local_i64("a", 42);
    value b = code.gen_local_i64("b", 42);
    value c = code.gen_local_i64("c", -42);
    code.gen_shl(a, 4);
    code.gen_shr(b, 2);
    code.gen_sha(c, 2);
    code.gen_call(test_shift, a, b, c);
    code.gen_ret();

    test();
}

i64 test_rot(void* ptr, i64 op1, i64 op2) {
    u8 a = (u8)op1;
    u8 b = (u8)op2;
    EXPECT_EQ(a, 81);
    EXPECT_EQ(b, 69);
    return 0;
}

TEST(cgen, rot) {
    cgen code(4 * KiB);

    func test = code.gen_function("test");
    value a = code.gen_local_i8("a", 42);
    value b = code.gen_local_i8("b", 42);
    code.gen_rol(a, 3);
    code.gen_ror(b, 3);
    code.gen_call(test_rot, a, b);
    code.gen_ret();

    test();
}

TEST(cgen, inc) {
    u64 global = 42;

    cgen code(4 * KiB);
    func test = code.gen_function("inc");
    value a = code.gen_local_i32("a", 42);
    value b = code.gen_global_i64("b", &global);
    code.gen_inc(a);
    code.gen_inc(b);
    code.gen_ret(a);

    EXPECT_EQ(test(), 43);
    EXPECT_EQ(global, 43);
}

TEST(cgen, dec) {
    u64 global = 42;

    cgen code(4 * KiB);
    func test = code.gen_function("dec");
    value a = code.gen_local_i32("a", 42);
    value b = code.gen_global_i64("b", &global);
    code.gen_dec(a);
    code.gen_dec(b);
    code.gen_ret(a);

    EXPECT_EQ(test(), 41);
    EXPECT_EQ(global, 41);
}

extern "C" int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
