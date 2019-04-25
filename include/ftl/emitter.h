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

#ifndef FTL_EMITTER_H
#define FTL_EMITTER_H

#include "ftl/common.h"
#include "ftl/error.h"
#include "ftl/bitops.h"

#include "ftl/reg.h"
#include "ftl/cache.h"

namespace ftl {

    class emitter
    {
    private:
        cache& m_code;

        size_t rex(bool is64, bool rexr, bool rexx, bool rexb);
        size_t modrm(int mod, int reg, int rm);
        size_t sib(int scale, int index, int base);

        size_t prefix(int bits, reg r, const rm& rm);
        size_t modrm(reg r, const rm& rm);

        size_t immop(int op, int bits, const rm& dest, i32 imm);
        size_t aluop(int op, int bits, const rm& dest, const rm& src);
        size_t shift(int op, int bits, const rm& dest, i8 imm);

        // disabled
        emitter();
        emitter(const emitter&);

    public:
        emitter(cache& code_cache);
        virtual ~emitter();

        size_t ret();

        size_t push(reg src);
        size_t pop(reg dest);

        size_t movi(int bits, const rm& dest, i64 imm);
        size_t addi(int bits, const rm& dest, i32 imm);
        size_t ori (int bits, const rm& dest, i32 imm);
        size_t adci(int bits, const rm& dest, i32 imm);
        size_t sbbi(int bits, const rm& dest, i32 imm);
        size_t andi(int bits, const rm& dest, i32 imm);
        size_t subi(int bits, const rm& dest, i32 imm);
        size_t xori(int bits, const rm& dest, i32 imm);
        size_t cmpi(int bits, const rm& dest, i32 imm);
        size_t tsti(int bits, const rm& dest, i32 imm);

        size_t movr(int bits, const rm& dest, const rm& src);
        size_t addr(int bits, const rm& dest, const rm& src);
        size_t orr (int bits, const rm& dest, const rm& src);
        size_t adcr(int bits, const rm& dest, const rm& src);
        size_t sbbr(int bits, const rm& dest, const rm& src);
        size_t andr(int bits, const rm& dest, const rm& src);
        size_t subr(int bits, const rm& dest, const rm& src);
        size_t xorr(int bits, const rm& dest, const rm& src);
        size_t cmpr(int bits, const rm& dest, const rm& src);
        size_t tstr(int bits, const rm& dest, const rm& src);

        size_t notr(int bits, const rm& dest);
        size_t negr(int bits, const rm& dest);

        size_t roli(int bits, const rm& dest, i8 imm);
        size_t rori(int bits, const rm& dest, i8 imm);
        size_t rcli(int bits, const rm& dest, i8 imm);
        size_t rcri(int bits, const rm& dest, i8 imm);
        size_t shli(int bits, const rm& dest, i8 imm);
        size_t shri(int bits, const rm& dest, i8 imm);
        size_t sari(int bits, const rm& dest, i8 imm);

        size_t call(void* fn);
        size_t jmpi(i32 offset);
        size_t jmpr(const rm& dest);
    };

}

#endif
