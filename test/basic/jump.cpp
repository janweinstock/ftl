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

TEST(jump, jmpi) {
    ftl::cbuf code(1 * ftl::KiB);
    ftl::emitter emitter(code);
    entry_func* fn = (entry_func*)code.get_code_ptr();

    emitter.movi(32, RAX, 0);
    emitter.jmpi(5 + 1);
    ASSERT_EQ(emitter.movi(32, RAX, 41), 5);
    ASSERT_EQ(emitter.ret(), 1);
    emitter.movi(32, RAX, 42);
    emitter.ret();

    EXPECT_EQ(fn(), 42);
}

TEST(jump, jmpr) {
    ftl::cbuf code(1 * ftl::KiB);
    ftl::emitter emitter(code);
    entry_func* fn = (entry_func*)code.get_code_ptr();

    ASSERT_EQ(emitter.movi(32, RAX, 0), 5);
    ASSERT_EQ(emitter.movi(64, R8, (i64)fn + 5 + 10 + 3 + 5 + 1), 10);
    ASSERT_EQ(emitter.jmpr(R8), 3);
    ASSERT_EQ(emitter.movi(32, RAX, 41), 5);
    ASSERT_EQ(emitter.ret(), 1);
    emitter.movi(32, RAX, 42);
    emitter.ret();

    EXPECT_EQ(fn(), 42);
}
