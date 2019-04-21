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

        OPCODE_IMM8   = 0x80, // r/m8  += imm8
        OPCODE_IMM32  = 0x81, // r/m32 += imm32
        OPCODE_IMM32S = 0x83, // r/m32 += imm8
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

    size_t emitter::regreg(int r1, int r2) {
        return modrm(MODRM_DIRECT, r1 & 7, r2 & 7);
    }

    size_t emitter::regmem(int r1, int base, size_t offset) {
        size_t len = 0;

        modrm_bits mode = find_best_mode(offset);
        len += modrm(mode, r1 & 7, base & 7);

        if ((base & 7) == 4) // rsp or r12
            len += sib(SCALE1, base & 7, base & 7);

        if (mode == MODRM_DISP32)
            len += m_code.write<i32>(offset);
        if (mode == MODRM_DISP8)
            len += m_code.write<i8>(offset);

        return len;
    }

    size_t emitter::immop(int op, int bits, reg dest, i32 imm) {
        size_t len = 0;

        if (bits == 16)
            len += m_code.write<u8>(0x66);
        if (bits == 64 || dest >= REG_R8)
            len += rex(bits == 64, false, false, dest >= REG_R8);

        int immlen = encode_size(imm); // 8, 16, 32 or 64bits
        FTL_ERROR_ON(immlen > 32, "immediate operand too big");
        FTL_ERROR_ON(immlen > bits, "immediate operand too big");

        u8 opcode = bits == 8 ? OPCODE_IMM8 : OPCODE_IMM32;
        if ((opcode == OPCODE_IMM32) && (immlen <= 8))
            opcode = OPCODE_IMM32S;
        else
            immlen = min(bits, 32);

        len += m_code.write(opcode);
        len += modrm(MODRM_DIRECT, op, dest);

        switch (immlen) {
        case  8: len += m_code.write<i8>(imm);  break;
        case 16: len += m_code.write<i16>(imm); break;
        case 32: len += m_code.write<i32>(imm); break;
        default:
            FTL_ERROR("cannot encode immediate with %d bits", immlen);
        }

        return len;
    }

    size_t emitter::immop(int op, int bits, reg base, size_t offset, i32 imm) {
        size_t len = 0;

        if (bits == 16)
            len += m_code.write<u8>(0x66);
        if (bits == 64 || base >= REG_R8)
            len += rex(bits == 64, false, false, base >= REG_R8);

        int immlen = encode_size(imm); // 8, 16, 32 or 64bits
        FTL_ERROR_ON(immlen > 32, "immediate operand too big");
        FTL_ERROR_ON(immlen > bits, "immediate operand too big");

        u8 opcode = bits == 8 ? OPCODE_IMM8 : OPCODE_IMM32;
        if ((opcode == OPCODE_IMM32) && (immlen <= 8))
            opcode = OPCODE_IMM32S;
        else
            immlen = min(bits, 32);

        len += m_code.write(opcode);
        len += regmem(op, base, offset);

        switch (immlen) {
        case  8: len += m_code.write<i8>(imm);  break;
        case 16: len += m_code.write<i16>(imm); break;
        case 32: len += m_code.write<i32>(imm); break;
        default:
            FTL_ERROR("cannot encode immediate with %d bits", immlen);
        }

        return len;
    }

    size_t emitter::ret() {
        m_code.write<u8>(OPCODE_RET);
        return sizeof(u8);
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

    size_t emitter::movi(reg dest, u64 imm) {
        size_t len = 0;
        len += rex(true, false, false, dest >= REG_R8);
        len += m_code.write<u8>(OPCODE_MOVI + (dest & 0x7));
        len += m_code.write(imm);
        return len;
    }

    size_t emitter::mov(reg dest, reg from) {
        if (dest == from)
            return 0;

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
        len += regmem(dest, base, offset);
        return len;
    }

    size_t emitter::mov(reg base, size_t offset, reg src) {
        size_t len = 0;
        len += rex(true, src >= REG_R8, false, base >= REG_R8);
        len += m_code.write<u8>(OPCODE_MOVR);
        len += regmem(src, base, offset);
        return len;
    }

    size_t emitter::addi(int bits, reg dest, i32 imm) {
        if (imm == 0)
            return 0;
        return immop(0, bits, dest, imm);
    }

    size_t emitter::ori(int bits, reg dest, i32 imm) {
        if(imm == 0)
            return 0;
        return immop(1, bits, dest, imm);
    }

    size_t emitter::adci(int bits, reg dest, i32 imm) {
        return immop(2, bits, dest, imm);
    }

    size_t emitter::sbbi(int bits, reg dest, i32 imm) {
        return immop(3, bits, dest, imm);
    }

    size_t emitter::andi(int bits, reg dest, i32 imm) {
        return immop(4, bits, dest, imm);
    }

    size_t emitter::subi(int bits, reg dest, i32 imm) {
        return immop(5, bits, dest, imm);
    }

    size_t emitter::xori(int bits, reg dest, i32 imm) {
        return immop(6, bits, dest, imm);
    }

    size_t emitter::cmpi(int bits, reg dest, i32 imm) {
        return immop(7, bits, dest, imm);
    }

    size_t emitter::addi(int bits, reg base, size_t offset, i32 imm) {
        return immop(0, bits, base, offset, imm);
    }

    size_t emitter::ori(int bits, reg base, size_t offset, i32 imm) {
        return immop(1, bits, base, offset, imm);
    }

    size_t emitter::adci(int bits, reg base, size_t offset, i32 imm) {
        return immop(2, bits, base, offset, imm);
    }

    size_t emitter::sbbi(int bits, reg base, size_t offset, i32 imm) {
        return immop(3, bits, base, offset, imm);
    }

    size_t emitter::andi(int bits, reg base, size_t offset, i32 imm) {
        return immop(4, bits, base, offset, imm);
    }

    size_t emitter::subi(int bits, reg base, size_t offset, i32 imm) {
        return immop(5, bits, base, offset, imm);
    }

    size_t emitter::xori(int bits, reg base, size_t offset, i32 imm) {
        return immop(6, bits, base, offset, imm);
    }

    size_t emitter::cmpi(int bits, reg base, size_t offset, i32 imm) {
        return immop(7, bits, base, offset, imm);
    }

}
