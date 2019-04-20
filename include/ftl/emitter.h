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

        // disabled
        emitter();
        emitter(const emitter&);

    public:
        emitter(cache& code_cache);
        virtual ~emitter();

        size_t ret();

        size_t movi(reg dest, u64 imm); // dest = imm
        size_t mov(reg dest, reg from); // dest = from
        size_t mov(reg dest, reg base, size_t offset); // dest = [base+offset]
        size_t mov(reg base, size_t offset, reg src);  // [base+offset] = src

        size_t push(reg src);
        size_t pop(reg dest);
    };

}

#endif
