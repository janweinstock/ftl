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

    func test("simple", 4 * KiB);
    value a = test.gen_local_i32("a", 2);
    value b = test.gen_global_i32("b", &val);
    value c = test.gen_local_i32("c", -2);
    test.gen_add(a, b);
    test.gen_sub(b, c);
    test.gen_ret(a);
    test.finish();

    int ret = test();

    EXPECT_EQ(ret, 42);
    EXPECT_EQ(val, 42);
}

TEST(cgen, jump) {
    int a = 40;
    int b = 42;

    func maxfn("maxfn", 4 * KiB);
    label less = maxfn.gen_label("less");

    value va = maxfn.gen_global_i32("a", &a);
    value vb = maxfn.gen_global_i32("b", &b);
    maxfn.gen_cmp(va, vb);
    maxfn.gen_jl(less);
    maxfn.gen_ret(va);
    less.place();
    maxfn.gen_ret(vb);
    maxfn.finish();

    int ret = maxfn();
    int ref = max(a, b);

    EXPECT_EQ(ret, ref);
}

TEST(cgen, func) {
    int a = 22;
    int b = 20;
    int c = 17;
    int d = 11;

    cbuf buffer(4 * KiB);

    func addfn = func("addfn", buffer);
    value va = addfn.gen_local_i32("va", a);
    value vb = addfn.gen_local_i32("vb", b);
    addfn.gen_add(va, vb);
    addfn.gen_ret(va);
    addfn.finish();

    func subfn("subfn", buffer);
    value vc = subfn.gen_local_i32("vc", c);
    value vd = subfn.gen_local_i32("vd", d);
    subfn.gen_sub(vc, vd);
    subfn.gen_ret(vc);
    subfn.finish();

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

    cbuf buffer(4 * KiB);

    func test_val("test_val", buffer);
    value vb = test_val.gen_global_i32("vb", &b);
    value vx = test_val.gen_local_i32("vx", a);
    value vy = test_val.gen_local_i32("vy", a);
    value vz = test_val.gen_local_i32("vz", a);
    test_val.gen_imul(vx, vb);
    test_val.gen_idiv(vy, vb);
    test_val.gen_imod(vz, vb);
    test_val.gen_call(test_muldiv, vx, vy, vz);
    test_val.gen_ret();
    test_val.finish();

    func test_imm("test_imm", buffer);
    value vx2 = test_imm.gen_local_i32("vx2", a);
    value vy2 = test_imm.gen_local_i32("vy2", a);
    value vz2 = test_imm.gen_local_i32("vz2", a);
    test_imm.gen_imul(vx2, b);
    test_imm.gen_idiv(vy2, b);
    test_imm.gen_imod(vz2, b);
    test_imm.gen_call(test_muldiv, vx2, vy2, vz2);
    test_imm.gen_ret();
    test_imm.finish();

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

    cbuf buffer(4 * KiB);

    func test_val("test_val", buffer);
    value vb = test_val.gen_global_i32("vb", &b);
    value vx = test_val.gen_local_i32("vx", a);
    value vy = test_val.gen_local_i32("vy", a);
    value vz = test_val.gen_local_i32("vz", a);
    test_val.gen_umul(vx, vb);
    test_val.gen_udiv(vy, vb);
    test_val.gen_umod(vz, vb);
    test_val.gen_call(test_umuldiv, vx, vy, vz);
    test_val.gen_ret();
    test_val.finish();

    func test_imm("test_imm", buffer);
    value vx2 = test_imm.gen_local_i32("vx2", a);
    value vy2 = test_imm.gen_local_i32("vy2", a);
    value vz2 = test_imm.gen_local_i32("vz2", a);
    test_imm.gen_umul(vx2, b);
    test_imm.gen_udiv(vy2, b);
    test_imm.gen_umod(vz2, b);
    test_imm.gen_call(test_umuldiv, vx2, vy2, vz2);
    test_imm.gen_ret();
    test_imm.finish();

    test_val();
    test_imm();
}

i64 test_notneg(void* ptr, i64 a, i64 b) {
    EXPECT_EQ(a, ~42);
    EXPECT_EQ(b, -42);
    return 0;
}

TEST(cgen, notneg) {
    func test("test", 4 * KiB);
    value va = test.gen_local_i64("va", 42);
    value vb = test.gen_local_i64("vb", 42);
    test.gen_not(va);
    test.gen_neg(vb);
    test.gen_call(test_notneg, va, vb);
    test.gen_ret();
    test.finish();

    test();
}

i64 test_shift(void* ptr, i64 a, i64 b, i64 c) {
    EXPECT_EQ(a, 42u << 4);
    EXPECT_EQ(b, 42u >> 2);
    EXPECT_EQ(c, -42 >> 2);
    return 0;
}

TEST(cgen, shift) {
    func test("test", 4 * KiB);
    value a = test.gen_local_i64("a", 42);
    value b = test.gen_local_i64("b", 42);
    value c = test.gen_local_i64("c", -42);
    test.gen_shl(a, 4);
    test.gen_shr(b, 2);
    test.gen_sha(c, 2);
    test.gen_call(test_shift, a, b, c);
    test.gen_ret();
    test.finish();

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
    func code("test", 4 * KiB);
    value a = code.gen_local_i8("a", 42);
    value b = code.gen_local_i8("b", 42);
    code.gen_rol(a, 3);
    code.gen_ror(b, 3);
    code.gen_call(test_rot, a, b);
    code.gen_ret();
    code.finish();

    code();
}

TEST(cgen, inc) {
    u64 global = 42;

    func code("inc", 4 * KiB);
    value a = code.gen_local_i32("a", 42);
    value b = code.gen_global_i64("b", &global);
    code.gen_inc(a);
    code.gen_inc(b);
    code.gen_ret(a);
    code.finish();

    EXPECT_EQ(code(), 43);
    EXPECT_EQ(global, 43);
}

TEST(cgen, dec) {
    u64 global = 42;

    func code("dec", 4 * KiB);
    value a = code.gen_local_i32("a", 42);
    value b = code.gen_global_i64("b", &global);
    code.gen_dec(a);
    code.gen_dec(b);
    code.gen_ret(a);
    code.finish();

    EXPECT_EQ(code(), 41);
    EXPECT_EQ(global, 41);
}

TEST(cgen, xchg) {
    u64 global1 = 11;
    u64 global2 = 22;

    func code("xchg", 4 * KiB);
    value a = code.gen_global_i64("a", &global1);
    value b = code.gen_global_i64("b", &global2);

    code.gen_xchg(a, b);
    code.gen_ret();
    code.finish();

    code();

    EXPECT_EQ(global1, 22);
    EXPECT_EQ(global2, 11);

}

TEST(code, mov32_64) {
    u64 global = 0x180000000;

    // make sure mov 64bit value to 32bit value uses zero extension
    func code("mov32_64", 4 * KiB);
    value a = code.gen_global_i64("a", &global);
    value b = code.gen_local_i32("b", ftl::RAX);
    a.fetch();
    code.gen_mov(b, a);
    code.free_value(a);
    code.free_value(b);
    code.gen_ret();
    code.finish();

    i64 res = code();

    EXPECT_EQ(res, 0x80000000);
}

TEST(code, far_variables) {
    u64 global1 = 42;
    u64 global2 = 24;

    func code("far_variables", 4 * KiB);
    code.set_data_ptr((void*)4); // forces use of extra base address register

    value a = code.gen_global_i64("a", &global1);
    value b = code.gen_global_i64("b", &global2);
    value t = code.gen_local_i64("t");

    EXPECT_FALSE(a.is_directly_addressable());
    EXPECT_FALSE(b.is_directly_addressable());

    code.gen_mov(t, a);
    code.gen_mov(a, b);
    code.gen_mov(b, t);

    code.gen_ret();
    code.finish();

    code.free_value(a);
    code.free_value(b);
    code.free_value(t);

    code();

    EXPECT_EQ(global1, 24);
    EXPECT_EQ(global2, 42);
}
