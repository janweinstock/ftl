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

typedef i32 (entry_func)(void);

TEST(fence, lfence) {
    ftl::cbuf code(1 * ftl::KiB);
    ftl::emitter emitter(code);
    entry_func* fn = (entry_func*)code.get_code_ptr();

    emitter.lfence();
    emitter.movi(32, RAX, 0);
    emitter.ret();

    int res = fn();
    EXPECT_EQ(res, 0);
}

TEST(fence, sfence) {
    ftl::cbuf code(1 * ftl::KiB);
    ftl::emitter emitter(code);
    entry_func* fn = (entry_func*)code.get_code_ptr();

    emitter.sfence();
    emitter.movi(32, RAX, 0);
    emitter.ret();

    int res = fn();
    EXPECT_EQ(res, 0);
}

TEST(fence, mfence) {
    ftl::cbuf code(1 * ftl::KiB);
    ftl::emitter emitter(code);
    entry_func* fn = (entry_func*)code.get_code_ptr();

    emitter.mfence();
    emitter.movi(32, RAX, 0);
    emitter.ret();

    int res = fn();
    EXPECT_EQ(res, 0);
}

extern "C" int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
