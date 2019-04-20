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

typedef int (entry_func)(void);

struct dummy {
    ftl::u64 a;
    ftl::u64 b;
    ftl::u64 c;
    ftl::u64 d;
};

TEST(emitter, mov) {
    ftl::cache code(1 * KiB);
    ftl::emitter emitter(code);

    emitter.movi(ftl::REG_RAX, 0);
    emitter.movi(ftl::REG_R15, 42);
    emitter.mov(ftl::REG_RAX, ftl::REG_R15);
    emitter.ret();

    entry_func* fn = (entry_func*)code.get_code_entry();
    EXPECT_EQ(fn(), 42);
}

TEST(emitter, load) {
    ftl::cache code(1 * KiB);
    ftl::emitter emitter(code);

    dummy data = { 42, 43, 44, 45 };

    entry_func* fn1 = (entry_func*)code.get_code_ptr();
    emitter.movi(ftl::REG_R8, (ftl::u64)&data);
    emitter.mov(ftl::REG_RAX, ftl::REG_R8, offsetof(dummy, a));
    emitter.ret();
    EXPECT_EQ(fn1(), data.a);

    entry_func* fn2 = (entry_func*)code.get_code_ptr();
    emitter.movi(ftl::REG_R8, (ftl::u64)&data);
    emitter.mov(ftl::REG_RAX, ftl::REG_R8, offsetof(dummy, b));
    emitter.ret();
    EXPECT_EQ(fn2(), data.b);

    entry_func* fn3 = (entry_func*)code.get_code_ptr();
    emitter.movi(ftl::REG_R11, (ftl::u64)&data);
    emitter.mov(ftl::REG_RAX, ftl::REG_R11, offsetof(dummy, c));
    emitter.ret();
    EXPECT_EQ(fn3(), data.c);

    entry_func* fn4 = (entry_func*)code.get_code_ptr();
    emitter.mov(ftl::REG_RCX, ftl::REG_RSP);
    emitter.movi(ftl::REG_RSP, (ftl::u64)&data);
    emitter.mov(ftl::REG_RAX, ftl::REG_RSP, offsetof(dummy, d));
    emitter.mov(ftl::REG_RSP, ftl::REG_RCX);
    emitter.ret();
    EXPECT_EQ(fn4(), data.d);
}

TEST(emitter, store) {
    ftl::cache code(1 * KiB);
    ftl::emitter emitter(code);

    dummy src = { 42, 43, 44, 45 };
    volatile dummy data = { 0, 0, 0, 0 };

    entry_func* fn1 = (entry_func*)code.get_code_ptr();
    emitter.movi(ftl::REG_RAX, src.a);
    emitter.movi(ftl::REG_RCX, (ftl::i64)&(data.a));
    emitter.mov(ftl::REG_RCX, offsetof(dummy, a), ftl::REG_RAX);
    emitter.ret();

    entry_func* fn2 = (entry_func*)code.get_code_ptr();
    emitter.movi(ftl::REG_RAX, src.b);
    emitter.movi(ftl::REG_RDX, (ftl::i64)&(data.a));
    emitter.mov(ftl::REG_RDX, offsetof(dummy, b), ftl::REG_RAX);
    emitter.ret();

    entry_func* fn3 = (entry_func*)code.get_code_ptr();
    emitter.movi(ftl::REG_RAX, src.c);
    emitter.movi(ftl::REG_R9, (ftl::i64)&(data.a));
    emitter.mov(ftl::REG_R9, offsetof(dummy, c), ftl::REG_RAX);
    emitter.ret();

    entry_func* fn4 = (entry_func*)code.get_code_ptr();
    emitter.movi(ftl::REG_RAX, src.d);
    emitter.movi(ftl::REG_R10, (ftl::i64)&(data.a));
    emitter.mov(ftl::REG_R10, offsetof(dummy, d), ftl::REG_RAX);
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
    ftl::cache code(1 * KiB);
    ftl::emitter emitter(code);

    entry_func* fn = (entry_func*)code.get_code_ptr();
    emitter.movi(ftl::REG_R10, 99);
    emitter.push(ftl::REG_R10);
    emitter.pop(ftl::REG_RAX);
    emitter.ret();

    EXPECT_EQ(fn(), 99);
}

extern "C" int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
