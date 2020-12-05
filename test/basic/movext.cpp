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
#include <limits.h>

#include "ftl.h"

using namespace ftl;
using namespace std;

typedef u64 (entry_func_u8)(u8);
typedef u64 (entry_func_u16)(u16);
typedef u64 (entry_func_u32)(u32);

TEST(mov, zeroext) {
    func cgen("zeroext", 4 * KiB);
    entry_func_u8* f1 = (entry_func_u8*)cgen.get_cbuffer().get_code_ptr();
    cgen.get_emitter().movi(64, RAX, -1);
    cgen.get_emitter().movzx(64, 8, RAX, argreg(0));
    cgen.get_emitter().ret();

    entry_func_u16* f2 = (entry_func_u16*)cgen.get_cbuffer().get_code_ptr();
    cgen.get_emitter().movi(64, RAX, -1);
    cgen.get_emitter().movzx(64, 16, RAX, argreg(0));
    cgen.get_emitter().ret();

    entry_func_u32* f3 = (entry_func_u32*)cgen.get_cbuffer().get_code_ptr();
    cgen.get_emitter().movi(64, RAX, -1);
    cgen.get_emitter().movzx(64, 32, RAX, argreg(0));
    cgen.get_emitter().ret();

    EXPECT_EQ(f1(0), 0);
    EXPECT_EQ(f1(-1), numeric_limits<u8>::max());
    EXPECT_EQ(f1(numeric_limits<u8>::min()), numeric_limits<u8>::min());
    EXPECT_EQ(f1(numeric_limits<u8>::max()), numeric_limits<u8>::max());

    EXPECT_EQ(f2(0), 0);
    EXPECT_EQ(f2(-1), numeric_limits<u16>::max());
    EXPECT_EQ(f2(numeric_limits<u16>::min()), numeric_limits<u16>::min());
    EXPECT_EQ(f2(numeric_limits<u16>::max()), numeric_limits<u16>::max());

    EXPECT_EQ(f3(0), 0);
    EXPECT_EQ(f3(-1), numeric_limits<u32>::max());
    EXPECT_EQ(f3(numeric_limits<u32>::min()), numeric_limits<u32>::min());
    EXPECT_EQ(f3(numeric_limits<u32>::max()), numeric_limits<u32>::max());
}

typedef i64 (entry_func_i8)(i8);
typedef i64 (entry_func_i16)(i16);
typedef i64 (entry_func_i32)(i32);

TEST(mov, signext) {
    func cgen("signext", 4 * KiB);
    entry_func_i8* f1 = (entry_func_i8*)cgen.get_cbuffer().get_code_ptr();
    cgen.get_emitter().movi(64, RAX, -1);
    cgen.get_emitter().movsx(64, 8, RAX, argreg(0));
    cgen.get_emitter().ret();

    entry_func_i16* f2 = (entry_func_i16*)cgen.get_cbuffer().get_code_ptr();
    cgen.get_emitter().movi(64, RAX, -1);
    cgen.get_emitter().movsx(64, 16, RAX, argreg(0));
    cgen.get_emitter().ret();

    entry_func_i32* f3 = (entry_func_i32*)cgen.get_cbuffer().get_code_ptr();
    cgen.get_emitter().movi(64, RAX, -1);
    cgen.get_emitter().movsx(64, 32, RAX, argreg(0));
    cgen.get_emitter().ret();

    EXPECT_EQ(f1(0), 0);
    EXPECT_EQ(f1(-1), -1);
    EXPECT_EQ(f1(numeric_limits<i8>::min()), numeric_limits<i8>::min());
    EXPECT_EQ(f1(numeric_limits<i8>::max()), numeric_limits<i8>::max());

    EXPECT_EQ(f2(0), 0);
    EXPECT_EQ(f2(-1), -1);
    EXPECT_EQ(f2(numeric_limits<i16>::min()), numeric_limits<i16>::min());
    EXPECT_EQ(f2(numeric_limits<i16>::max()), numeric_limits<i16>::max());

    EXPECT_EQ(f3(0), 0);
    EXPECT_EQ(f3(-1), -1);
    EXPECT_EQ(f3(numeric_limits<i32>::min()), numeric_limits<i32>::min());
    EXPECT_EQ(f3(numeric_limits<i32>::max()), numeric_limits<i32>::max());
}

TEST(code, signext8) {
    i8 v1 = -1;
    i64 v2 = 0;

    func code("signext8", 4 * KiB);
    value val1 = code.gen_global_i8("v1", &v1);
    value val2 = code.gen_global_i64("v2", &v2);
    code.gen_sxt(val2, val1);
    code.gen_ret(val2);
    code.finish();

    EXPECT_EQ(code.exec(), -1ll);
    EXPECT_EQ(v2, -1ll);
}

TEST(code, signext16) {
    i16 v1 = -1;
    i64 v2 = 0;

    func code("signext16", 4 * KiB);
    value val1 = code.gen_global_i16("v1", &v1);
    value val2 = code.gen_global_i64("v2", &v2);
    code.gen_sxt(val2, val1);
    code.gen_ret(val2);
    code.finish();

    EXPECT_EQ(code.exec(), -1ll);
    EXPECT_EQ(v2, -1ll);
}

TEST(code, signext32) {
    i32 v1 = -1;
    i64 v2 = 0;

    func code("signext", 4 * KiB);
    value val1 = code.gen_global_i32("v1", &v1);
    value val2 = code.gen_global_i64("v2", &v2);
    code.gen_sxt(val2, val1);
    code.gen_ret(val2);
    code.finish();

    EXPECT_EQ(code.exec(), -1ll);
    EXPECT_EQ(v2, -1ll);
}

TEST(code, signext_same8) {
    i64 v1 = 0x80;

    func code("signext_same8", 4 * KiB);
    value val1 = code.gen_global_i64("v1", &v1);
    code.gen_sxt(val1, val1, 64, 8);
    code.gen_ret(val1);
    code.finish();

    EXPECT_EQ(code.exec(), std::numeric_limits<i8>::lowest());
    EXPECT_EQ(v1, std::numeric_limits<i8>::lowest());
}

TEST(code, signext_same16) {
    i64 v1 = 0x8000;
    func code("signext_same16", 4 * KiB);
    value val1 = code.gen_global_i64("v1", &v1);
    code.gen_sxt(val1, val1, 64, 16);
    code.gen_ret(val1);
    code.finish();

    EXPECT_EQ(code.exec(), std::numeric_limits<i16>::lowest());
    EXPECT_EQ(v1, std::numeric_limits<i16>::lowest());
}

TEST(code, signext_same32) {
    i64 v1 = 0x80000000;

    func code("signext_same32", 4 * KiB);
    value val1 = code.gen_global_i64("v1", &v1);
    code.gen_sxt(val1, val1, 64, 32);
    code.gen_ret(val1);
    code.finish();

    EXPECT_EQ(code.exec(), std::numeric_limits<i32>::lowest());
    EXPECT_EQ(v1, std::numeric_limits<i32>::lowest());
}

TEST(code, zeroext8) {
    i64 v1 = ~0ull;
    i64 v2 = 0;

    func code("zeroext8", 4 * KiB);
    value val1 = code.gen_global_i8("v1", &v1);
    value val2 = code.gen_global_i64("v2", &v2);
    code.gen_zxt(val2, val1);
    code.gen_ret(val2);
    code.finish();

    code.exec();

    EXPECT_EQ(v2, 0xff);
}

TEST(code, zeroext16) {
    i64 v1 = ~0ull;
    i64 v2 = 0;

    func code("zeroext8", 4 * KiB);
    value val1 = code.gen_global_i16("v1", &v1);
    value val2 = code.gen_global_i64("v2", &v2);
    code.gen_zxt(val2, val1);
    code.gen_ret(val2);
    code.finish();

    code.exec();

    EXPECT_EQ(v2, 0xffff);
}

TEST(code, zeroext32) {
    i64 v1 = ~0ull;
    i64 v2 = 0;

    func code("zeroext32", 4 * KiB);
    value val1 = code.gen_global_i32("v1", &v1);
    value val2 = code.gen_global_i64("v2", &v2);
    code.gen_zxt(val2, val1);
    code.gen_ret(val2);
    code.finish();

    code.exec();

    EXPECT_EQ(v2, 0xffffffff);
}

TEST(code, zeroext_same8) {
    i64 v = ~0ull;

    func code("zeroext_same8", 4 * KiB);
    value val = code.gen_global_i64("v", &v);
    code.gen_zxt(val, val, 64, 8);
    code.gen_ret(val);
    code.finish();

    code.exec();

    EXPECT_EQ(v, 0xff);
}

TEST(code, zeroext_same16) {
    i64 v = ~0ull;

    func code("zeroext_same16", 4 * KiB);
    value val = code.gen_global_i64("v", &v);
    code.gen_zxt(val, val, 64, 16);
    code.gen_ret(val);
    code.finish();

    code.exec();

    EXPECT_EQ(v, 0xffff);
}

TEST(code, zeroext_same32) {
    i64 v = ~0ull;

    func code("zeroext_same32", 4 * KiB);
    value val = code.gen_global_i64("v", &v);
    code.gen_zxt(val, val, 64, 32);
    code.gen_ret(val);
    code.finish();

    code.exec();

    EXPECT_EQ(v, 0xffffffff);
}
