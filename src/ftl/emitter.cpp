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
        OPCODE_RET    = 0xc3,

        OPCODE_PUSH   = 0x50,
        OPCODE_POP    = 0x58,

        OPCODE_MOVIR  = 0xb0, // reg <- imm
        OPCODE_MOVIRM = 0xc6, // r/m <- imm
        OPCODE_MOVSXD = 0x63, // reg <- signext(r/m32)

        OPCODE_IMM8   = 0x80, // r/m8  += imm8
        OPCODE_IMM32  = 0x81, // r/m32 += imm32
        OPCODE_IMM32S = 0x83, // r/m32 += imm8

        OPCODE_SHIFT  = 0xc0, // shift group
        OPCODE_SHIFT1 = 0xd0, // shift by one

        OPCODE_ADD    = 0x00,
        OPCODE_OR     = 0x08,
        OPCODE_ADC    = 0x10,
        OPCODE_SBB    = 0x18,
        OPCODE_AND    = 0x20,
        OPCODE_SUB    = 0x28,
        OPCODE_XOR    = 0x30,
        OPCODE_CMP    = 0x38,
        OPCODE_TST    = 0x84,
        OPCODE_MOV    = 0x88,
        OPCODE_UNARY  = 0xf6,

        OPCODE_IMUL8  = 0x6b,
        OPCODE_IMUL32 = 0x69,

        OPCODE_CALL   = 0xe8,
        OPCODE_JMPI   = 0xeb,
        OPCODE_JMPR   = 0xff,

        OPCODE_BRANCH = 0x70,

        OPCODE_ESCAPE = 0x0f,
    };

    enum opcode_2bytes {
        OPCODE2_BR32  = 0x80,
        OPCODE2_SET   = 0x90,
        OPCODE2_IMUL  = 0xaf,
        OPCODE2_MOVZX = 0xb6,
        OPCODE2_MOVSX = 0xbe,
    };

    enum opcode_imm {
        OPCODE_IMM_ADD = 0,
        OPCODE_IMM_OR  = 1,
        OPCODE_IMM_ADC = 2,
        OPCODE_IMM_SBB = 3,
        OPCODE_IMM_AND = 4,
        OPCODE_IMM_SUB = 5,
        OPCODE_IMM_XOR = 6,
        OPCODE_IMM_CMP = 7,
    };

    enum opcode_shift {
        OPCODE_SHIFT_ROL = 0,
        OPCODE_SHIFT_ROR = 1,
        OPCODE_SHIFT_RCL = 2,
        OPCODE_SHIFT_RCR = 3,
        OPCODE_SHIFT_SHL = 4,
        OPCODE_SHIFT_SHR = 5,
        OPCODE_SHIFT_SAR = 7,
    };

    enum opcode_unary {
        OPCODE_UNARY_TEST = 0,
        OPCODE_UNARY_NOT  = 2,
        OPCODE_UNARY_NEG  = 3,
        OPCODE_UNARY_MUL  = 4,
        OPCODE_UNARY_IMUL = 5,
        OPCODE_UNARY_DIV  = 6,
        OPCODE_UNARY_IDIV = 7,
    };

    enum branch_condition {
        BRCOND_O  = 0x0, // jump if overflow
        BRCOND_NO = 0x1, // jump if no overflow
        BRCOND_B  = 0x2, // jump if below
        BRCOND_AE = 0x3, // jump if above or equal
        BRCOND_Z  = 0x4, // jump if zero
        BRCOND_NZ = 0x5, // jump if not zero
        BRCOND_BE = 0x6, // jump if below or equal
        BRCOND_A  = 0x7, // jump if above
        BRCOND_S  = 0x8, // jump if sign
        BRCOND_NS = 0x9, // jump if no sign
        BRCOND_P  = 0xa, // jump if parity even
        BRCOND_NP = 0xb, // jump if parity odd
        BRCOND_L  = 0xc, // jump if less than
        BRCOND_GE = 0xd, // jump if greater or equal
        BRCOND_LE = 0xe, // jump if less or equal
        BRCOND_G  = 0xf, // jump if greater than
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

    enum scale {
        SCALE1 = 0,
        SCALE2 = 1,
        SCALE4 = 2,
        SCALE8 = 3,
    };

    void emitter::setup_fixup(fixup* fix, int size) {
        if (fix) {
            fix->code = m_buffer.get_code_ptr();
            fix->size = size;
        }
    }

    size_t emitter::rex(bool is64, bool rexr, bool rexx, bool rexb) {
        u8 rex = REX_BASE;
        if (is64) rex |= REX_W;
        if (rexr) rex |= REX_R;
        if (rexx) rex |= REX_X;
        if (rexb) rex |= REX_B;
        return m_buffer.write(rex);
    }

    size_t emitter::modrm(int mod, int reg, int rm) {
        u8 modrm = ((mod & 3) << 6) | ((reg & 7) << 3) | (rm & 7);
        return m_buffer.write(modrm);
    }

    size_t emitter::sib(int scale, int index, int base) {
        u8 sib = ((scale & 3) << 6) | ((index & 7) << 3) | (base & 7);
        return m_buffer.write(sib);
    }

    size_t emitter::prefix(int bits, reg r, const rm& rm) {
        size_t len = 0;
        if (bits == 16)
            len += m_buffer.write<u8>(0x66);
        if (bits == 8 || bits == 64 || r >= R8 || rm.r >= R8)
            len += rex(bits == 64, r >= R8, false, rm.r >= R8);
        return len;
    }

    size_t emitter::modrm(reg r, const rm& rm) {
        if (!rm.is_mem)
            return modrm(MODRM_DIRECT, r & 7, rm.r & 7);

        size_t len = 0;
        modrm_bits mode;

        if (rm.offset == 0)
            mode = MODRM_INDIRECT;
        else if (fits_i8(rm.offset))
            mode = MODRM_DISP8;
        else
            mode = MODRM_DISP32;

        len += modrm(mode, r & 7, rm.r & 7);

        if ((rm.r & 7) == 4) // special case: rsp and r12 need extra sib
            len += sib(SCALE1, rm.r & 7, rm.r & 7);

        if (mode == MODRM_DISP32)
            len += m_buffer.write<i32>(rm.offset);
        if (mode == MODRM_DISP8)
            len += m_buffer.write<i8>(rm.offset);

        return len;
    }

    size_t emitter::immop(int op, int bits, const rm& dest, i32 imm) {
        int immlen = encode_size(imm); // 8, 16 or 32bits
        FTL_ERROR_ON(immlen > bits, "immediate operand too big");
        FTL_ERROR_ON(bits > 64, "requested operation too wide");

        u8 opcode = bits == 8 ? OPCODE_IMM8 : OPCODE_IMM32;
        if ((opcode == OPCODE_IMM32) && (immlen <= 8))
            opcode = OPCODE_IMM32S;
        else
            immlen = min(bits, 32);

        size_t len = 0;
        len += prefix(bits, (reg)0, dest);
        len += m_buffer.write(opcode);
        len += modrm((reg)op, dest);

        switch (immlen) {
        case  8: len += m_buffer.write<i8>(imm);  break;
        case 16: len += m_buffer.write<i16>(imm); break;
        case 32: len += m_buffer.write<i32>(imm); break;
        default:
            FTL_ERROR("cannot encode immediate with %d bits", immlen);
        }

        return len;
    }

    size_t emitter::aluop(int op, int bits, const rm& dest, const rm& src) {
        if (dest.is_mem && src.is_mem)
            FTL_ERROR("source and destination cannot both be in memory");
        FTL_ERROR_ON(bits > 64, "requested operation too wide");

        rm oprm(src.is_mem ? src : dest); // operand used for modrm.rm
        rm op_r(src.is_mem ? dest : src); // operand used for modrm.reg

        u8 opcode = op;
        if (bits > 8)
            opcode += 1;
        if (src.is_mem)
            opcode += 2;

        size_t len = 0;
        len += prefix(bits, op_r.r, oprm);
        len += m_buffer.write(opcode);
        len += modrm(op_r.r, oprm);

        return len;
    }

    size_t emitter::shift(int op, int bits, const rm& dest, u8 imm) {
        FTL_ERROR_ON(bits > 64, "requested operation too wide");
        FTL_ERROR_ON(imm >= bits, "cannot shift by %d", (int)imm);

        if (imm == 0)
            return 0;

        u8 opcode = imm == 1 ? OPCODE_SHIFT1 : OPCODE_SHIFT;
        if (bits > 8)
            opcode++;

        size_t len = 0;
        len += prefix(bits, (reg)0, dest);
        len += m_buffer.write(opcode);
        len += modrm((reg)op, dest);

        if (imm != 1)
            len += m_buffer.write(imm);

        return len;
    }

    size_t emitter::branch(int op, i32 imm, fixup* fix) {
        size_t len = 0;

        if (fits_i8(imm)) {
            len += m_buffer.write<u8>(OPCODE_BRANCH + op);
            setup_fixup(fix, 1);
            len += m_buffer.write<i8>(imm);
        } else {
            len += m_buffer.write<u8>(OPCODE_ESCAPE);
            len += m_buffer.write<u8>(OPCODE2_BR32 + op);
            setup_fixup(fix, 4);
            len += m_buffer.write<i32>(imm);
        }

        return len;
    }

    size_t emitter::setcc(int op, const rm& dest) {
        size_t len = 0;

        len += prefix(8, (reg)0, dest);
        len += m_buffer.write<u8>(OPCODE_ESCAPE);
        len += m_buffer.write<u8>(OPCODE2_SET + op);
        len += modrm((reg)0, dest);

        return len;
    }

    emitter::emitter(cbuf& code):
        m_buffer(code) {
    }

    emitter::~emitter() {
        // nothing to do
    }

    size_t emitter::ret() {
        return m_buffer.write<u8>(OPCODE_RET);
    }

    size_t emitter::push(reg src) {
        size_t len = 0;
        if (src >= R8)
            len += rex(false, false, false, true);
        len += m_buffer.write<u8>(OPCODE_PUSH + (src & 7));
        return len;
    }

    size_t emitter::pop(reg dest) {
        size_t len = 0;
        if (dest >= R8)
            len += rex(false, false, false, true);
        len += m_buffer.write<u8>(OPCODE_POP + (dest & 7));
        return len;
    }

    size_t emitter::movi(int bits, const rm& dest, i64 imm) {
        if (imm == 0 && dest.is_reg())
            return xorr(bits, dest, dest);

        int immlen = max(encode_size(imm), bits);
        FTL_ERROR_ON(bits > 64, "requested operation too wide");
        FTL_ERROR_ON(immlen > bits, "immediate operand too big");
        FTL_ERROR_ON(dest.is_mem && immlen > 32, "immediate operand too big");

        size_t len = 0;
        len += prefix(bits, (reg)0, dest);

        if (dest.is_reg()) {
            u8 opcode = (bits == 8) ? OPCODE_MOVIR : (OPCODE_MOVIR + 8);
            len += m_buffer.write<u8>(opcode + (dest.r & 7));
        } else {
            u8 opcode = (bits == 8) ? OPCODE_MOVIRM : (OPCODE_MOVIRM + 1);
            len += m_buffer.write<u8>(opcode);
            len += modrm((reg)0, dest);
        }

        switch (immlen) {
        case  8: len += m_buffer.write<i8> (imm); break;
        case 16: len += m_buffer.write<i16>(imm); break;
        case 32: len += m_buffer.write<i32>(imm); break;
        case 64: len += m_buffer.write<i64>(imm); break;
        default:
            FTL_ERROR("cannot encode immediate with %d bits", immlen);
        }

        return len;
    }

    size_t emitter::addi(int bits, const rm& dest, i32 imm) {
        if (imm == 0)
            return 0;
        return immop(OPCODE_IMM_ADD, bits, dest, imm);
    }

    size_t emitter::ori(int bits, const rm& dest, i32 imm) {
        if(imm == 0)
            return 0;
        return immop(OPCODE_IMM_OR, bits, dest, imm);
    }

    size_t emitter::adci(int bits, const rm& dest, i32 imm) {
        return immop(OPCODE_IMM_ADC, bits, dest, imm);
    }

    size_t emitter::sbbi(int bits, const rm& dest, i32 imm) {
        return immop(OPCODE_IMM_SBB, bits, dest, imm);
    }

    size_t emitter::andi(int bits, const rm& dest, i32 imm) {
        return immop(OPCODE_IMM_AND, bits, dest, imm);
    }

    size_t emitter::subi(int bits, const rm& dest, i32 imm) {
        return immop(OPCODE_IMM_SUB, bits, dest, imm);
    }

    size_t emitter::xori(int bits, const rm& dest, i32 imm) {
        return immop(OPCODE_IMM_XOR, bits, dest, imm);
    }

    size_t emitter::cmpi(int bits, const rm& dest, i32 imm) {
        return immop(OPCODE_IMM_CMP, bits, dest, imm);
    }

    size_t emitter::tsti(int bits, const rm& dest, i32 imm) {
        int immlen = encode_size(imm); // 8, 16 or 32bits
        FTL_ERROR_ON(bits > 64, "requested operation too wide");
        FTL_ERROR_ON(immlen > bits, "immediate operand too big");
        FTL_ERROR_ON(immlen > 32, "immediate operand too big");

        u8 opcode = OPCODE_UNARY;
        if (bits > 8)
            opcode++;

        size_t len = 0;
        reg r = (reg)OPCODE_UNARY_TEST;
        len += prefix(bits, r, dest);
        len += m_buffer.write(opcode);
        len += modrm(r, dest);

        switch (bits) {
        case  8: len += m_buffer.write<i8>(imm);  break;
        case 16: len += m_buffer.write<i16>(imm); break;
        case 32: len += m_buffer.write<i32>(imm); break;
        case 64: len += m_buffer.write<i32>(imm); break;
        default:
            FTL_ERROR("cannot encode immediate with %d bits", immlen);
        }

        return len;
    }

    size_t emitter::movr(int bits, const rm& dest, const rm& src) {
        if (dest.is_reg() && src.is_reg() && dest.r == src.r)
            return 0;
        return aluop(OPCODE_MOV, bits, dest, src);
    }

    size_t emitter::addr(int bits, const rm& dest, const rm& src) {
        return aluop(OPCODE_ADD, bits, dest, src);
    }

    size_t emitter::orr (int bits, const rm& dest, const rm& src) {
        return aluop(OPCODE_OR, bits, dest, src);
    }

    size_t emitter::adcr(int bits, const rm& dest, const rm& src) {
        return aluop(OPCODE_ADC, bits, dest, src);
    }

    size_t emitter::sbbr(int bits, const rm& dest, const rm& src) {
        return aluop(OPCODE_SBB, bits, dest, src);
    }

    size_t emitter::andr(int bits, const rm& dest, const rm& src) {
        return aluop(OPCODE_AND, bits, dest, src);
    }

    size_t emitter::subr(int bits, const rm& dest, const rm& src) {
        return aluop(OPCODE_SUB, bits, dest, src);
    }

    size_t emitter::xorr(int bits, const rm& dest, const rm& src) {
        return aluop(OPCODE_XOR, bits, dest, src);
    }

    size_t emitter::cmpr(int bits, const rm& dest, const rm& src) {
        return aluop(OPCODE_CMP, bits, dest, src);
    }

    size_t emitter::tstr(int bits, const rm& dest, const rm& src) {
        // we must make sure that op2 is a register, otherwise aluop will
        // compute an invalid opcode
        const rm& op1(dest.is_mem ? dest : src);
        const rm& op2(dest.is_mem ? src : dest);
        return aluop(OPCODE_TST, bits, op1, op2);
    }

    size_t emitter::notr(int bits, const rm& op) {
        return aluop(OPCODE_UNARY, bits, op, (reg)OPCODE_UNARY_NOT);
    }

    size_t emitter::negr(int bits, const rm& op) {
        return aluop(OPCODE_UNARY, bits, op, (reg)OPCODE_UNARY_NEG);
    }

    size_t emitter::mulr(int bits, const rm& op) {
        return aluop(OPCODE_UNARY, bits, op, (reg)OPCODE_UNARY_MUL);
    }

    size_t emitter::imul(int bits, const rm& op) {
        return aluop(OPCODE_UNARY, bits, op, (reg)OPCODE_UNARY_IMUL);
    }

    size_t emitter::divr(int bits, const rm& op) {
        return aluop(OPCODE_UNARY, bits, op, (reg)OPCODE_UNARY_DIV);
    }

    size_t emitter::idiv(int bits, const rm& op) {
        return aluop(OPCODE_UNARY, bits, op, (reg)OPCODE_UNARY_IDIV);
    }

    size_t emitter::imuli(int bits, reg dest, const rm& src, i32 imm) {
        int immlen = encode_size(imm);
        FTL_ERROR_ON(bits < 16, "8bit multiplication not supported");
        FTL_ERROR_ON(immlen > bits, "immediate too big to encode");

        size_t len = 0;
        len += prefix(bits, dest, src);

        u8 opcode = (immlen == 8) ? OPCODE_IMUL8 : OPCODE_IMUL32;
        len += m_buffer.write(opcode);
        len += modrm(dest, src);

        if (immlen == 8)
            len += m_buffer.write<i8>(imm);
        else
            len += m_buffer.write<i32>(imm);

        return len;
    }

    size_t emitter::imulr(int bits, reg dest, const rm& src) {
        FTL_ERROR_ON(bits < 16, "8bit multiplication not supported");

        size_t len = 0;
        len += prefix(bits, dest, src);
        len += m_buffer.write<u8>(OPCODE_ESCAPE);
        len += m_buffer.write<u8>(OPCODE2_IMUL);
        len += modrm(dest, src);

        return len;
    }

    size_t emitter::roli(int bits, const rm& dest, u8 imm) {
        return shift(OPCODE_SHIFT_ROL, bits, dest, imm);
    }

    size_t emitter::rori(int bits, const rm& dest, u8 imm) {
        return shift(OPCODE_SHIFT_ROR, bits, dest, imm);
    }

    size_t emitter::rcli(int bits, const rm& dest, u8 imm) {
        return shift(OPCODE_SHIFT_RCL, bits, dest, imm);
    }

    size_t emitter::rcri(int bits, const rm& dest, u8 imm) {
        return shift(OPCODE_SHIFT_RCR, bits, dest, imm);
    }

    size_t emitter::shli(int bits, const rm& dest, u8 imm) {
        return shift(OPCODE_SHIFT_SHL, bits, dest, imm);
    }

    size_t emitter::shri(int bits, const rm& dest, u8 imm) {
        return shift(OPCODE_SHIFT_SHR, bits, dest, imm);
    }

    size_t emitter::sari(int bits, const rm& dest, u8 imm) {
        return shift(OPCODE_SHIFT_SAR, bits, dest, imm);
    }

    size_t emitter::movzx(int dbits, int sbits, const rm& dest, const rm& src) {
        FTL_ERROR_ON(dbits < sbits, "source wider than destination");
        FTL_ERROR_ON(dbits == sbits, "attempt to extend to same width");

        if (dest.is_mem)
           FTL_ERROR("destination must be register");

        if (sbits == 32)
            return movr(sbits, dest, src);

        size_t len = 0;
        len += prefix(dbits, dest.r, src);

        u8 opcode = OPCODE2_MOVZX;
        if (sbits == 16)
            opcode++;

        len += m_buffer.write<u8>(OPCODE_ESCAPE);
        len += m_buffer.write<u8>(opcode);
        len += modrm(dest.r, src);

        return len;
    }

    size_t emitter::movsx(int dbits, int sbits, const rm& dest, const rm& src) {
        FTL_ERROR_ON(dbits < sbits, "source wider than destination");
        FTL_ERROR_ON(dbits == sbits, "attempt to extend to same width");

        if (dest.is_mem)
           FTL_ERROR("destination must be register");

        size_t len = 0;
        len += prefix(dbits, dest.r, src);

        switch (sbits) {
        case 32:
            len += m_buffer.write<u8>(OPCODE_MOVSXD);
            break;

        case 16:
            len += m_buffer.write<u8>(OPCODE_ESCAPE);
            len += m_buffer.write<u8>(OPCODE2_MOVSX + 1);
            break;

        case 8:
            len += m_buffer.write<u8>(OPCODE_ESCAPE);
            len += m_buffer.write<u8>(OPCODE2_MOVSX);
            break;

        default:
            FTL_ERROR("invalid source operand width: %d bits", sbits);
        }

        len += modrm(dest.r, src);
        return len;
    }

    size_t emitter::call(u8* fn, fixup* fix) {
        if ((fn == NULL) && (fix != NULL))
            fn = m_buffer.get_code_ptr();

        i64 offset = fn - m_buffer.get_code_ptr() - 5;
        if (!fits_i32(offset))
            FTL_ERROR("cannot call %p, out of reach", fn);

        size_t len = 0;
        len += m_buffer.write<u8>(OPCODE_CALL);
        setup_fixup(fix, 4);
        len += m_buffer.write<i32>(offset);
        return len;
    }

    size_t emitter::call(const rm& dest) {
        size_t len = 0;
        len += prefix(32, (reg)0, dest);
        len += m_buffer.write<u8>(OPCODE_JMPR);
        len += modrm((reg)2, dest);
        return len;
    }

    size_t emitter::jmpi(i32 offset, fixup* fix) {
        size_t len = 0;

        if (fits_i8(offset)) {
            len += m_buffer.write<u8>(OPCODE_JMPI);
            setup_fixup(fix, 1);
            len += m_buffer.write<i8>(offset);
        } else {
            len += m_buffer.write<u8>(OPCODE_JMPI - 2);
            setup_fixup(fix, 4);
            len += m_buffer.write<i32>(offset);
        }

        return len;
    }

    size_t emitter::jmpr(const rm& dest) {
        size_t len = 0;
        len += prefix(32, (reg)0, dest);
        len += m_buffer.write<u8>(OPCODE_JMPR);
        len += modrm((reg)4, dest);
        return len;
    }

    size_t emitter::jo(i32 offset, fixup* fix) {
        return branch(BRCOND_O, offset, fix);
    }

    size_t emitter::jno(i32 offset, fixup* fix) {
        return branch(BRCOND_NO, offset, fix);
    }

    size_t emitter::jb(i32 offset, fixup* fix) {
        return branch(BRCOND_B, offset, fix);
    }

    size_t emitter::jae(i32 offset, fixup* fix) {
        return branch(BRCOND_AE, offset, fix);
    }

    size_t emitter::jz(i32 offset, fixup* fix) {
        return branch(BRCOND_Z, offset, fix);
    }

    size_t emitter::jnz(i32 offset, fixup* fix) {
        return branch(BRCOND_NZ, offset, fix);
    }

    size_t emitter::je(i32 offset, fixup* fix) {
        return branch(BRCOND_Z, offset, fix);
    }

    size_t emitter::jne(i32 offset, fixup* fix) {
        return branch(BRCOND_NZ, offset, fix);
    }

    size_t emitter::jbe(i32 offset, fixup* fix) {
        return branch(BRCOND_BE, offset, fix);
    }

    size_t emitter::ja(i32 offset, fixup* fix) {
        return branch(BRCOND_A, offset, fix);
    }

    size_t emitter::js(i32 offset, fixup* fix) {
        return branch(BRCOND_S, offset, fix);
    }

    size_t emitter::jns(i32 offset, fixup* fix) {
        return branch(BRCOND_NS, offset, fix);
    }

    size_t emitter::jp(i32 offset, fixup* fix) {
        return branch(BRCOND_P, offset, fix);
    }

    size_t emitter::jnp(i32 offset, fixup* fix) {
        return branch(BRCOND_NP, offset, fix);
    }

    size_t emitter::jl(i32 offset, fixup* fix) {
        return branch(BRCOND_L, offset, fix);
    }

    size_t emitter::jge(i32 offset, fixup* fix) {
        return branch(BRCOND_GE, offset, fix);
    }

    size_t emitter::jle(i32 offset, fixup* fix) {
        return branch(BRCOND_LE, offset, fix);
    }

    size_t emitter::jg(i32 offset, fixup* fix) {
        return branch(BRCOND_G, offset, fix);
    }

    size_t emitter::seto(const rm& dest) {
        return setcc(BRCOND_O, dest);
    }

    size_t emitter::setno(const rm& dest) {
        return setcc(BRCOND_NO, dest);
    }

    size_t emitter::setb(const rm& dest) {
        return setcc(BRCOND_B, dest);
    }

    size_t emitter::setae(const rm& dest) {
        return setcc(BRCOND_AE, dest);
    }

    size_t emitter::setz(const rm& dest) {
        return setcc(BRCOND_Z, dest);
    }

    size_t emitter::setnz(const rm& dest) {
        return setcc(BRCOND_NZ, dest);
    }

    size_t emitter::sete(const rm& dest) {
        return setcc(BRCOND_Z, dest);
    }

    size_t emitter::setne(const rm& dest) {
        return setcc(BRCOND_NZ, dest);
    }

    size_t emitter::setbe(const rm& dest) {
        return setcc(BRCOND_BE, dest);
    }

    size_t emitter::seta(const rm& dest) {
        return setcc(BRCOND_A, dest);
    }

    size_t emitter::sets(const rm& dest) {
        return setcc(BRCOND_S, dest);
    }

    size_t emitter::setns(const rm& dest) {
        return setcc(BRCOND_NS, dest);
    }

    size_t emitter::setp(const rm& dest) {
        return setcc(BRCOND_P, dest);
    }

    size_t emitter::setnp(const rm& dest) {
        return setcc(BRCOND_NP, dest);
    }

    size_t emitter::setl(const rm& dest) {
        return setcc(BRCOND_L, dest);
    }

    size_t emitter::setge(const rm& dest) {
        return setcc(BRCOND_GE, dest);
    }

    size_t emitter::setle(const rm& dest) {
        return setcc(BRCOND_LE, dest);
    }

    size_t emitter::setg(const rm& dest) {
        return setcc(BRCOND_G, dest);
    }

}
