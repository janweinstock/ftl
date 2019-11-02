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

TEST(muldiv, imuli) {
    ftl::cbuf code(1 * ftl::KiB);
    ftl::emitter emitter(code);

    typedef int (test_func)(int);
    test_func* imul = (test_func*)code.get_code_ptr();
    emitter.imuli(32, R8, argreg(0), 5);
    emitter.movr(32, RAX, R8);
    emitter.ret();

    EXPECT_EQ(imul(1), 5);
    EXPECT_EQ(imul(2), 10);
    EXPECT_EQ(imul(-1), -5);
}

TEST(muldiv, imulr) {
    ftl::cbuf code(1 * ftl::KiB);
    ftl::emitter emitter(code);

    typedef int (test_func)(int, int);
    test_func* imul = (test_func*)code.get_code_ptr();
    emitter.movr(32, R8, argreg(0));
    emitter.imulr(32, R8, argreg(1));
    emitter.movr(32, RAX, R8);
    emitter.ret();

    EXPECT_EQ(imul(1,  1), 1);
    EXPECT_EQ(imul(2, -2), -4);
    EXPECT_EQ(imul(-1, -1), 1);
}

TEST(muldiv, imul) {
    ftl::cbuf code(1 * ftl::KiB);
    ftl::emitter emitter(code);

    typedef int (test_func)(int, int);
    test_func* imul = (test_func*)code.get_code_ptr();
    emitter.movr(32, RAX, argreg(0));
    emitter.imul(32, argreg(1));
    emitter.ret();

    EXPECT_EQ(imul(1,  1), 1);
    EXPECT_EQ(imul(2, -2), -4);
    EXPECT_EQ(imul(-1, -1), 1);
}

TEST(muldiv, idiv) {
    ftl::cbuf code(1 * ftl::KiB);
    ftl::emitter emitter(code);

    typedef int (test_func)(int, int);
    test_func* idiv = (test_func*)code.get_code_ptr();
    emitter.xorr(32, RDX, RDX);
    emitter.movr(32, RAX, argreg(0));
    emitter.idiv(32, argreg(1));
    emitter.ret();

    EXPECT_EQ(idiv(16,  4), 4);
    EXPECT_EQ(idiv(27, -3), -9);
    EXPECT_EQ(idiv(20, 11), 1);
}

TEST(muldiv, imod) {
    ftl::cbuf code(1 * ftl::KiB);
    ftl::emitter emitter(code);

    typedef int (test_func)(int, int);
    test_func* imod = (test_func*)code.get_code_ptr();
    emitter.xorr(32, RDX, RDX);
    emitter.movr(32, RAX, argreg(0));
    emitter.idiv(32, argreg(1));
    emitter.movr(32, RAX, RDX);
    emitter.ret();

    EXPECT_EQ(imod(16,  4), 16 %  4);
    EXPECT_EQ(imod(27, -4), 27 % -4);
    EXPECT_EQ(imod(30, 11), 30 % 11);
}

TEST(muldiv, mul) {
    ftl::cbuf code(1 * ftl::KiB);
    ftl::emitter emitter(code);

    typedef unsigned int (test_func)(unsigned int, unsigned int);
    test_func* mul = (test_func*)code.get_code_ptr();
    emitter.movr(32, RAX, argreg(0));
    emitter.imul(32, argreg(1));
    emitter.ret();

    EXPECT_EQ(mul(1, 1), 1);
    EXPECT_EQ(mul(2, 2), 4);
    EXPECT_EQ(mul(255, 197), 255 * 197);
}

TEST(muldiv, div) {
    ftl::cbuf code(1 * ftl::KiB);
    ftl::emitter emitter(code);

    typedef int (test_func)(int, int);
    test_func* div = (test_func*)code.get_code_ptr();
    emitter.xorr(32, RDX, RDX);
    emitter.movr(32, RAX, argreg(0));
    emitter.idiv(32, argreg(1));
    emitter.ret();

    EXPECT_EQ(div(16,  4), 4);
    EXPECT_EQ(div(27,  3), 9);
    EXPECT_EQ(div(20, 11), 1);
}

TEST(muldiv, mod) {
    ftl::cbuf code(1 * ftl::KiB);
    ftl::emitter emitter(code);

    typedef int (test_func)(int, int);
    test_func* mod = (test_func*)code.get_code_ptr();
    emitter.xorr(32, RDX, RDX);
    emitter.movr(32, RAX, argreg(0));
    emitter.idiv(32, argreg(1));
    emitter.movr(32, RAX, RDX);
    emitter.ret();

    EXPECT_EQ(mod(16,  4), 16 % 4);
    EXPECT_EQ(mod(27, 27), 27 % 27);
    EXPECT_EQ(mod(30, 11), 30 % 11);
}
