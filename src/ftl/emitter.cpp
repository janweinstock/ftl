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

#include "ftl/emitter.h"

namespace ftl {

    enum opcode {
        FTL_OPCODE_RET  = 0xc3,

        FTL_OPCODE_MOVI = 0xb8,
    };

    emitter::emitter(cache& code):
        m_code(code) {
    }

    emitter::~emitter() {
        // nothing to do
    }

    size_t emitter::rex(bool is64, bool rexr, bool rexx, bool rexb) {
        u8 rex = 0x40;
        if (is64) // rex.w: whether operation is 64bit)
            rex |= 8;
        if (rexr) // rex.r: whether the ModR/M REG field refers to r8-r15
            rex |= 4;
        if (rexx) // rex.x: whether the ModR/M SIB index field refers to r8-r15
            rex |= 2;
        if (rexb) // rex.b: whether the ModR/M RM or SIB refers to r8-r15
            rex |= 1;

        return m_code.write(rex);
    }

    size_t emitter::ret() {
        m_code.write<u8>(FTL_OPCODE_RET);
        return sizeof(u8);
    }

    size_t emitter::movi(reg r, u64 imm) {
        size_t len = 0;

        len += rex(true, false, false, r >= REG_R8);
        len += m_code.write<u8>(FTL_OPCODE_MOVI + (r & 0x7));
        len += m_code.write(imm);

        return len;
    }

}
