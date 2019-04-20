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

TEST(bitops, fits) {
    size_t val0 = 0;
    EXPECT_TRUE(ftl::fits_i8(val0));

    size_t val1 = 127;
    EXPECT_TRUE(ftl::fits_i8(val1));

    size_t val2 = 128;
    EXPECT_FALSE(ftl::fits_i8(val2));

    size_t val3 = -128;
    EXPECT_TRUE(ftl::fits_i8(val3));

    size_t val4 = -128;
    EXPECT_TRUE(ftl::fits_i8(val4));
}

extern "C" int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
