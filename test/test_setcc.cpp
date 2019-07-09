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

TEST(muldiv, setcc) {
    ftl::cbuf code(1 * ftl::KiB);
    ftl::emitter emitter(code);

    typedef int (test_func)(int, int);
    test_func* test = (test_func*)code.get_code_ptr();
    emitter.movi(32, RAX, 0);
    emitter.cmpr(32, argreg(0), argreg(1));
    emitter.setz(RAX);
    emitter.ret();

    EXPECT_EQ(test(1, 1), 1);
    EXPECT_EQ(test(1, 0), 0);
    EXPECT_EQ(test(0, 1), 0);
    EXPECT_EQ(test(0, 0), 1);
}



extern "C" int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
