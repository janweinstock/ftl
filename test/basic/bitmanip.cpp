/******************************************************************************
 *                                                                            *
 * Copyright 2020 Jan Henrik Weinstock                                        *
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

#ifndef __COUNTER__
#define __COUNTER__ __LINE__
#endif

#define _CONCAT(x, y) x##y
#define CONCAT(x, y)  _CONCAT(x,y)
#define UNIQUE(name)  CONCAT(name, __COUNTER__)

#define MKTEST_SET_IMM(_bits, _val, _idx)                                     \
    TEST(bitmanip, UNIQUE(bts_imm ## _bits ## _ ## _idx ## _)) {              \
        u##_bits global = _val;                                               \
        func code("bts");                                                     \
        value res = code.gen_scratch_val("result", _bits, 0, RAX);            \
        value val = code.gen_global_val("val", _bits, &global);               \
        code.gen_bts(val, _idx);                                              \
        code.gen_setb(res);                                                   \
        code.gen_ret(res);                                                    \
        i64 bit = code();                                                     \
        EXPECT_EQ(global, _val | (1ul << _idx));                              \
        EXPECT_EQ(bit, (_val >> _idx) & 1);                                   \
    }

#define MKTEST_SET_REG(_bits, _val, _idx)                                     \
    TEST(bitmanip, UNIQUE(bts_reg ## _bits ## _ ## _idx ## _)) {              \
        u##_bits global = _val;                                               \
        func code("bts");                                                     \
        value res = code.gen_scratch_val("result", _bits, 0, RAX);            \
        value rxx = code.gen_scratch_val("rxx", _bits, _idx, NREGS);          \
        value val = code.gen_global_val("val", _bits, &global);               \
        code.gen_bts(val, rxx);                                               \
        code.gen_setb(res);                                                   \
        code.gen_ret(res);                                                    \
        i64 bit = code();                                                     \
        EXPECT_EQ(global, _val | (1ul << _idx));                              \
        EXPECT_EQ(bit, (_val >> _idx) & 1);                                   \
    }

#define MKTEST_SET(_bits, _val, _idx) \
    MKTEST_SET_IMM(_bits, _val, _idx) \
    MKTEST_SET_REG(_bits, _val, _idx)

MKTEST_SET(32, 0x00000000, 0);
MKTEST_SET(32, 0x00000005, 3);
MKTEST_SET(32, 0x7fffffff, 31);
MKTEST_SET(64, 0x0000000000000000ul, 0);
MKTEST_SET(64, 0x0000010000000000ul, 40);
MKTEST_SET(64, 0x7ffffffffffffffful, 63);

#define MKTEST_RESET_IMM(_bits, _val, _idx)                                   \
    TEST(bitmanip, UNIQUE(btr_imm ## _bits ## _ ## _idx ## _)) {              \
        u##_bits global = _val;                                               \
        func code("btr");                                                     \
        value res = code.gen_scratch_val("result", _bits, 0, RAX);            \
        value val = code.gen_global_val("val", _bits, &global);               \
        code.gen_btr(val, _idx);                                              \
        code.gen_setb(res);                                                   \
        code.gen_ret(res);                                                    \
        i64 bit = code();                                                     \
        EXPECT_EQ(global, _val & ~(1ul << _idx));                             \
        EXPECT_EQ(bit, (_val >> _idx) & 1);                                   \
    }

#define MKTEST_RESET_REG(_bits, _val, _idx)                                   \
    TEST(bitmanip, UNIQUE(btr_reg ## _bits ## _ ## _idx ## _)) {              \
        u##_bits global = _val;                                               \
        func code("btr");                                                     \
        value res = code.gen_scratch_val("result", _bits, 0, RAX);            \
        value rxx = code.gen_scratch_val("rxx", _bits, _idx, NREGS);          \
        value val = code.gen_global_val("val", _bits, &global);               \
        code.gen_btr(val, rxx);                                               \
        code.gen_setb(res);                                                   \
        code.gen_ret(res);                                                    \
        i64 bit = code();                                                     \
        EXPECT_EQ(global, _val & ~(1ul << _idx));                             \
        EXPECT_EQ(bit, (_val >> _idx) & 1);                                   \
    }

#define MKTEST_RESET(_bits, _val, _idx) \
    MKTEST_RESET_IMM(_bits, _val, _idx) \
    MKTEST_RESET_REG(_bits, _val, _idx)

MKTEST_RESET(32, 0x00000001, 0);
MKTEST_RESET(32, 0x00000005, 2);
MKTEST_RESET(32, 0x7fffffff, 31);
MKTEST_RESET(64, 0x0000000000000001ul, 0);
MKTEST_RESET(64, 0x0000010000000000ul, 40);
MKTEST_RESET(64, 0x8ffffffffffffffful, 63);
