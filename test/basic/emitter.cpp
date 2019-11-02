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

typedef int (entry_func)(void);

struct dummy {
    u64 a;
    u64 b;
    u64 c;
    u64 d;
};

TEST(emitter, mov) {
    cbuf code(1 * KiB);
    emitter emitter(code);

    emitter.movi(64, RAX, 0ll);
    emitter.movi(64, R15, 42ll);
    emitter.movr(64, RAX, R15);
    emitter.ret();

    entry_func* fn = (entry_func*)code.get_code_entry();
    EXPECT_EQ(fn(), 42);
}

TEST(emitter, load) {
    cbuf code(1 * KiB);
    emitter emitter(code);

    dummy data = { 42, 43, 44, 45 };

    entry_func* fn1 = (entry_func*)code.get_code_ptr();
    emitter.movi(64, R8, (i64)&data);
    emitter.movr(64, RAX, memop(R8, (i64)offsetof(dummy, a)));
    emitter.ret();
    EXPECT_EQ(fn1(), data.a);

    entry_func* fn2 = (entry_func*)code.get_code_ptr();
    emitter.movi(64, R8, (i64)&data);
    emitter.movr(64, RAX, memop(R8, (i64)offsetof(dummy, b)));
    emitter.ret();
    EXPECT_EQ(fn2(), data.b);

    entry_func* fn3 = (entry_func*)code.get_code_ptr();
    emitter.movi(64, R11, (i64)&data);
    emitter.movr(64, RAX, memop(R11, (i64)offsetof(dummy, c)));
    emitter.ret();
    EXPECT_EQ(fn3(), data.c);

    entry_func* fn4 = (entry_func*)code.get_code_ptr();
    emitter.movr(64, RCX, RSP);
    emitter.movi(64, RSP, (i64)&data);
    emitter.movr(64, RAX, memop(RSP, (i64)offsetof(dummy, d)));
    emitter.movr(64, RSP, RCX);
    emitter.ret();
    EXPECT_EQ(fn4(), data.d);
}

TEST(emitter, store) {
    cbuf code(1 * KiB);
    emitter emitter(code);

    dummy src = { 42, 43, 44, 45 };
    volatile dummy data = { 0, 0, 0, 0 };

    entry_func* fn1 = (entry_func*)code.get_code_ptr();
    emitter.movi(64, RAX, src.a);
    emitter.movi(64, RCX, (i64)&(data.a));
    emitter.movr(64, memop(RCX, offsetof(dummy, a)), RAX);
    emitter.ret();

    entry_func* fn2 = (entry_func*)code.get_code_ptr();
    emitter.movi(64, RAX, src.b);
    emitter.movi(64, RDX, (i64)&(data.a));
    emitter.movr(64, memop(RDX, (i64)offsetof(dummy, b)), RAX);
    emitter.ret();

    entry_func* fn3 = (entry_func*)code.get_code_ptr();
    emitter.movi(64, RAX, src.c);
    emitter.movi(64, R9, (i64)&(data.a));
    emitter.movr(64, memop(R9, (i64)offsetof(dummy, c)), RAX);
    emitter.ret();

    entry_func* fn4 = (entry_func*)code.get_code_ptr();
    emitter.movi(64, RAX, src.d);
    emitter.movi(64, R10, (i64)&(data.a));
    emitter.movr(64, memop(R10, (i64)offsetof(dummy, d)), RAX);
    emitter.ret();

    EXPECT_EQ(fn1(), src.a);
    EXPECT_EQ(fn2(), src.b);
    EXPECT_EQ(fn3(), src.c);
    EXPECT_EQ(fn4(), src.d);

    EXPECT_EQ(data.a, src.a);
    EXPECT_EQ(data.b, src.b);
    EXPECT_EQ(data.c, src.c);
    EXPECT_EQ(data.d, src.d);
}

TEST(emitter, pushpop) {
    cbuf code(1 * KiB);
    emitter emitter(code);

    entry_func* fn = (entry_func*)code.get_code_ptr();
    emitter.movi(64, R10, 99ll);
    emitter.push(R10);
    emitter.pop(RAX);
    emitter.ret();

    EXPECT_EQ(fn(), 99);
}

TEST(emitter, notr) {
    cbuf code(1 * KiB);
    emitter emitter(code);

    i32 val = 0xaaaaaaaa;

    entry_func* fn = (entry_func*)code.get_code_ptr();
    emitter.movi(32, RAX, val);
    emitter.notr(32, RAX);
    emitter.ret();

    EXPECT_EQ(fn(), ~val);
}

TEST(emitter, negr) {
    cbuf code(1 * KiB);
    emitter emitter(code);

    i32 val = -42;

    entry_func* fn = (entry_func*)code.get_code_ptr();
    emitter.movi(32, RAX, val);
    emitter.negr(32, RAX);
    emitter.ret();

    EXPECT_EQ(fn(), -val);
}
