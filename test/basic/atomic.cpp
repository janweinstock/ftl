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

#define MKTEST(bits, dval, sval, cval)                                        \
    TEST(atomic, cmpxchg_ ## bits ## _ ## dval ## _ ## sval ## _ ## cval) {   \
        ftl::i ## bits global = dval;                                         \
        ftl::func code("atomic.cmpxchg", 4 * KiB);                            \
        ftl::value dest = code.gen_global_val("atomic.dest", bits, &global);  \
        ftl::value src  = code.gen_local_val("atomic.src", bits, sval);       \
        ftl::value cmpv = code.gen_local_val("atomic.cmpv", bits, cval);      \
        ftl::value succ = code.gen_local_val("atomic.succ", bits);            \
        code.gen_cmpxchg(dest, src, cmpv);                                    \
        code.free_value(cmpv);                                                \
        code.free_value(src);                                                 \
        code.gen_setz(succ);                                                  \
        code.gen_ret(succ);                                                   \
        i64 ret = code.exec();                                                \
        EXPECT_EQ(global, dval == cval ? sval : dval);                        \
        EXPECT_EQ(ret, dval == cval);                                         \
    }

MKTEST(8,  0,  0,  1);
MKTEST(8,  1,  0,  1);
MKTEST(8, 42, 11, 42);
MKTEST(8, 21, 15, 20);

MKTEST(16,  0,  0,  1);
MKTEST(16,  1,  0,  1);
MKTEST(16, 42, 12, 42);
MKTEST(16, 21, 16, 20);

MKTEST(32,  0,  0,  1);
MKTEST(32,  1,  0,  1);
MKTEST(32, 42, 13, 42);
MKTEST(32, 21, 17, 20);

MKTEST(64,  0,  0,  1);
MKTEST(64,  1,  0,  1);
MKTEST(64, 42, 14, 42);
MKTEST(64, 21, 18, 20);
