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
        OPCODE_RET  = 0xc3,

        OPCODE_PUSH = 0x50,
        OPCODE_POP  = 0x58,

        OPCODE_MOVI = 0xb8, // reg <- imm
        OPCODE_MOVR = 0x89, // reg <- reg
        OPCODE_MOVM = 0x8b, // reg <- mem
    };

    enum rex_bits {
        REX_BASE = 1 << 6,
        REX_W    = 1 << 3, // whether operation is 64bit
        REX_R    = 1 << 2, // whether the ModR/M REG field refers to r8-r15
        REX_X    = 1 << 1, // whether the ModR/M SIB index refers to r8-r15
        REX_B    = 1 << 0, // whether the ModR/M RM or SIB refers to r8-r15
    };

    enum modrm_bits {
        MODRM_INDIRECT = 0,
        MODRM_DISP8  = 1,
        MODRM_DISP32 = 2,
        MODRM_DIRECT = 3,
    };

    static modrm_bits find_best_mode(size_t offset) {
        if (offset == 0)
            return MODRM_INDIRECT;
        if (fits_i8(offset))
            return MODRM_DISP8;
        if (fits_i32(offset))
            return MODRM_DISP32;
        FTL_ERROR("address offset exceeds limit");
        return MODRM_DIRECT;
    }

    enum scale {
        SCALE1 = 0,
        SCALE2 = 1,
        SCALE4 = 2,
        SCALE8 = 3,
    };

    emitter::emitter(cache& code):
        m_code(code) {
    }

    emitter::~emitter() {
        // nothing to do
    }

    size_t emitter::rex(bool is64, bool rexr, bool rexx, bool rexb) {
        u8 rex = REX_BASE;
        if (is64) rex |= REX_W;
        if (rexr) rex |= REX_R;
        if (rexx) rex |= REX_X;
        if (rexb) rex |= REX_B;
        return m_code.write(rex);
    }

    size_t emitter::modrm(int mod, int reg, int rm) {
        u8 modrm = ((mod & 3) << 6) | ((reg & 7) << 3) | (rm & 7);
        return m_code.write(modrm);
    }

    size_t emitter::sib(int scale, int index, int base) {
        u8 sib = ((scale & 3) << 6) | ((index & 7) << 3) | (base & 7);
        return m_code.write(sib);
    }

    size_t emitter::ret() {
        m_code.write<u8>(OPCODE_RET);
        return sizeof(u8);
    }

    size_t emitter::movi(reg dest, u64 imm) {
        size_t len = 0;
        len += rex(true, false, false, dest >= REG_R8);
        len += m_code.write<u8>(OPCODE_MOVI + (dest & 0x7));
        len += m_code.write(imm);
        return len;
    }

    size_t emitter::mov(reg dest, reg from) {
        size_t len = 0;
        len += rex(true, from >= REG_R8, false, dest >= REG_R8);
        len += m_code.write<u8>(OPCODE_MOVR);
        len += modrm(MODRM_DIRECT, from & 7, dest & 7);
        return len;
    }

    size_t emitter::mov(reg dest, reg base, size_t offset) {
        size_t len = 0;
        len += rex(true, dest >= REG_R8, false, base >= REG_R8);
        len += m_code.write<u8>(OPCODE_MOVM);

        modrm_bits mode = find_best_mode(offset);
        len += modrm(mode, dest & 7, base & 7);

        if ((base & 7) == 4) // rsp or r12
            len += sib(SCALE1, base & 7, base & 7);

        if (mode == MODRM_DISP32)
            len += m_code.write<i32>(offset);
        if (mode == MODRM_DISP8)
            len += m_code.write<i8>(offset);

        return len;
    }

    size_t emitter::mov(reg base, size_t offset, reg src) {
        size_t len = 0;
        len += rex(true, src >= REG_R8, false, base >= REG_R8);
        len += m_code.write<u8>(OPCODE_MOVR);

        modrm_bits mode = find_best_mode(offset);
        len += modrm(mode, src & 7, base & 7);

        if ((base & 7) == 4) // rsp or r12
            len += sib(SCALE1, base & 7, base & 7);

        if (mode == MODRM_DISP32)
            len += m_code.write<i32>(offset);
        if (mode == MODRM_DISP8)
            len += m_code.write<i8>(offset);

        return len;
    }

    size_t emitter::push(reg src) {
        size_t len = 0;
        if (src >= REG_R8)
            len += rex(true, false, false, true);
        len += m_code.write<u8>(OPCODE_PUSH + (src & 7));
        return len;
    }

    size_t emitter::pop(reg dest) {
        size_t len = 0;
        if (dest >= REG_R8)
            len += rex(true, false, false, true);
        len += m_code.write<u8>(OPCODE_POP + (dest & 7));
        return len;
    }

}
