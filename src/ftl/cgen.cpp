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

#include "ftl/cgen.h"

namespace ftl {

    void cgen::gen_entry_exit() {
        u8* head = m_buffer.get_code_entry();
        u8* code = m_buffer.get_code_ptr();
        FTL_ERROR_ON(code != head, "entry code must be written to head");

        if (!m_entry.is_placed())
            m_entry.place();

        m_alloc.prologue();
        m_buffer.align(16);

        if (!m_exit.is_placed())
            m_exit.place();

        m_alloc.epilogue();
        m_buffer.align(16);
    }

    cgen::cgen(size_t size):
        m_buffer(size),
        m_emitter(m_buffer),
        m_alloc(m_emitter),
        m_entry(m_buffer),
        m_exit(m_buffer) {
    }

    cgen::~cgen() {
        // nothing to do
    }

    void cgen::reset() {
        m_alloc.reset();
        m_buffer.reset();
        gen_entry_exit();
    }

    void cgen::set_base_ptr_stack() {
        int dummy;
        set_base_ptr(&dummy);
    }

    void cgen::set_base_ptr_heap() {
        int* dummy = new int;
        set_base_ptr(dummy);
        delete dummy;
    }

    void cgen::set_base_ptr_code() {
        set_base_ptr(m_buffer.get_code_ptr());
    }

    func cgen::gen_function() {
        if (!m_entry.is_placed() || !m_exit.is_placed())
            gen_entry_exit();
        return func(m_buffer);
    }

    void cgen::gen_ret() {
        gen_jmp(m_exit, true);
    }

    void cgen::gen_ret(i64 val) {
        m_emitter.movi(64, RAX, val);
        gen_ret();
    }

    void cgen::gen_ret(value& val) {
        m_alloc.fetch(val, RAX);
        gen_ret();
    }

    void cgen::gen_jmp(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_emitter.jmpi(offset, &fix);
        l.add(fix);
    }

    void cgen::gen_jo(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_emitter.jo(offset, &fix);
        l.add(fix);
    }

    void cgen::gen_jno(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_emitter.jno(offset, &fix);
        l.add(fix);
    }

    void cgen::gen_jb(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_emitter.jb(offset, &fix);
        l.add(fix);
    }

    void cgen::gen_jae(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_emitter.jae(offset, &fix);
        l.add(fix);
    }

    void cgen::gen_jz(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_emitter.jz(offset, &fix);
        l.add(fix);
    }

    void cgen::gen_jnz(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_emitter.jnz(offset, &fix);
        l.add(fix);
    }

    void cgen::gen_je(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_emitter.je(offset, &fix);
        l.add(fix);
    }

    void cgen::gen_jne(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_emitter.jne(offset, &fix);
        l.add(fix);
    }

    void cgen::gen_jbe(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_emitter.jbe(offset, &fix);
        l.add(fix);
    }

    void cgen::gen_ja(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_emitter.ja(offset, &fix);
        l.add(fix);
    }

    void cgen::gen_js(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_emitter.js(offset, &fix);
        l.add(fix);
    }

    void cgen::gen_jns(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_emitter.jns(offset, &fix);
        l.add(fix);
    }

    void cgen::gen_jp(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_emitter.jp(offset, &fix);
        l.add(fix);
    }

    void cgen::gen_jnp(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_emitter.jnp(offset, &fix);
        l.add(fix);
    }

    void cgen::gen_jl(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_emitter.jl(offset, &fix);
        l.add(fix);
    }

    void cgen::gen_jge(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_emitter.jge(offset, &fix);
        l.add(fix);
    }

    void cgen::gen_jle(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_emitter.jle(offset, &fix);
        l.add(fix);
    }

    void cgen::gen_jg(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_emitter.jg(offset, &fix);
        l.add(fix);
    }

    void cgen::gen_seto(value& dest) {
        if (dest.is_mem())
            m_alloc.assign(m_alloc.select(), &dest);

        m_emitter.seto(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void cgen::gen_setno(value& dest) {
        if (dest.is_mem())
            m_alloc.assign(m_alloc.select(), &dest);

        m_emitter.setno(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void cgen::gen_setb(value& dest) {
        if (dest.is_mem())
            m_alloc.assign(m_alloc.select(), &dest);

        m_emitter.setb(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void cgen::gen_setae(value& dest) {
        if (dest.is_mem())
            m_alloc.assign(m_alloc.select(), &dest);

        m_emitter.setae(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void cgen::gen_setz(value& dest) {
        if (dest.is_mem())
            m_alloc.assign(m_alloc.select(), &dest);

        m_emitter.setz(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void cgen::gen_setnz(value& dest) {
        if (dest.is_mem())
            m_alloc.assign(m_alloc.select(), &dest);

        m_emitter.setnz(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void cgen::gen_sete(value& dest) {
        if (dest.is_mem())
            m_alloc.assign(m_alloc.select(), &dest);

        m_emitter.sete(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void cgen::gen_setne(value& dest) {
        if (dest.is_mem())
            m_alloc.assign(m_alloc.select(), &dest);

        m_emitter.setne(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void cgen::gen_setbe(value& dest) {
        if (dest.is_mem())
            m_alloc.assign(m_alloc.select(), &dest);

        m_emitter.setbe(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void cgen::gen_seta(value& dest) {
        if (dest.is_mem())
            m_alloc.assign(m_alloc.select(), &dest);

        m_emitter.seta(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void cgen::gen_sets(value& dest) {
        if (dest.is_mem())
            m_alloc.assign(m_alloc.select(), &dest);

        m_emitter.sets(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void cgen::gen_setns(value& dest) {
        if (dest.is_mem())
            m_alloc.assign(m_alloc.select(), &dest);

        m_emitter.setns(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void cgen::gen_setp(value& dest) {
        if (dest.is_mem())
            m_alloc.assign(m_alloc.select(), &dest);

        m_emitter.setp(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void cgen::gen_setnp(value& dest) {
        if (dest.is_mem())
            m_alloc.assign(m_alloc.select(), &dest);

        m_emitter.setnp(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void cgen::gen_setl(value& dest) {
        if (dest.is_mem())
            m_alloc.assign(m_alloc.select(), &dest);

        m_emitter.setl(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void cgen::gen_setge(value& dest) {
        if (dest.is_mem())
            m_alloc.assign(m_alloc.select(), &dest);

        m_emitter.setge(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void cgen::gen_setle(value& dest) {
        if (dest.is_mem())
            m_alloc.assign(m_alloc.select(), &dest);

        m_emitter.setle(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void cgen::gen_setg(value& dest) {
        if (dest.is_mem())
            m_alloc.assign(m_alloc.select(), &dest);

        m_emitter.setg(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void cgen::gen_mov(value& dest, const value& src) {
        if (dest == src)
            return;

        if (dest.is_mem() && src.is_mem())
            m_alloc.assign(m_alloc.select(), &dest);

        if (dest.bits < src.bits)
            m_emitter.movzx(dest.bits, src.bits, dest, src);
        else
            m_emitter.movr(dest.bits, dest, src);

        dest.mark_dirty();
    }

    void cgen::gen_add(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            dest.fetch();
        m_emitter.addr(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void cgen::gen_or(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            dest.fetch();
        m_emitter.orr(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void cgen::gen_adc(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            m_alloc.fetch(dest);
        m_emitter.adcr(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void cgen::gen_sbb(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            dest.fetch();
        m_emitter.sbbr(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void cgen::gen_and(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            dest.fetch();
        m_emitter.andr(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void cgen::gen_sub(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            dest.fetch();
        m_emitter.subr(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void cgen::gen_xor(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            dest.fetch();
        m_emitter.xorr(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void cgen::gen_cmp(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            dest.fetch();
        m_emitter.cmpr(dest.bits, dest, src);
    }

    void cgen::gen_tst(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            dest.fetch();
        m_emitter.tstr(dest.bits, dest, src);
    }

    void cgen::gen_mov(value& dest, i64 val) {
        int immlen = max(encode_size(val), dest.bits);
        if (dest.is_mem() && immlen > 32)
            m_alloc.assign(m_alloc.select(), &dest);

        m_emitter.movi(dest.bits, dest, val);
        dest.mark_dirty();
    }

    void cgen::gen_add(value& dest, i32 val) {
        m_emitter.addi(dest.bits, dest, val);
        dest.mark_dirty();
    }

    void cgen::gen_or(value& dest, i32 val) {
        m_emitter.ori(dest.bits, dest, val);
        dest.mark_dirty();
    }

    void cgen::gen_adc(value& dest, i32 val) {
        m_emitter.adci(dest.bits, dest, val);
        dest.mark_dirty();
    }

    void cgen::gen_sbb(value& dest, i32 val) {
        m_emitter.sbbi(dest.bits, dest, val);
        dest.mark_dirty();
    }

    void cgen::gen_and(value& dest, i32 val) {
        m_emitter.andi(dest.bits, dest, val);
        dest.mark_dirty();
    }

    void cgen::gen_sub(value& dest, i32 val) {
        m_emitter.subi(dest.bits, dest, val);
        dest.mark_dirty();
    }

    void cgen::gen_xor(value& dest, i32 val) {
        m_emitter.xori(dest.bits, dest, val);
        dest.mark_dirty();
    }

    void cgen::gen_cmp(value& dest, i32 val) {
        m_emitter.cmpi(dest.bits, dest, val);
        dest.mark_dirty();
    }

    void cgen::gen_tst(value& dest, i32 val) {
        m_emitter.tsti(dest.bits, dest, val);
        dest.mark_dirty();
    }

    void cgen::gen_imul(value& dest, const value& src) {
        if (dest.r == RAX) {
            m_emitter.imul(dest.bits, src);
        } else {
            m_alloc.fetch(dest);
            m_emitter.imulr(dest.bits, dest.r, src);
        }

        dest.mark_dirty();
    }

    void cgen::gen_idiv(value& dest, const value& src) {
        m_alloc.fetch(dest, RAX);
        m_alloc.flush(RDX);
        m_emitter.xorr(32, RDX, RDX);
        m_emitter.idiv(dest.bits, src);
        dest.mark_dirty();
    }

    void cgen::gen_imod(value& dest, const value& src) {
        gen_idiv(dest, src);
        m_alloc.assign(RDX, &dest);
    }

    void cgen::gen_umul(value& dest, const value& src) {
        m_alloc.fetch(dest, RAX);
        m_alloc.flush(RDX);
        m_emitter.mulr(dest.bits, src);
        dest.mark_dirty();
    }

    void cgen::gen_udiv(value& dest, const value& src) {
        m_alloc.fetch(dest, RAX);
        m_alloc.flush(RDX);
        m_emitter.xorr(32, RDX, RDX);
        m_emitter.divr(dest.bits, src);
        dest.mark_dirty();
    }

    void cgen::gen_umod(value& dest, const value& src) {
        gen_udiv(dest, src);
        m_alloc.assign(RDX, &dest);
    }

    void cgen::gen_imul(value& dest, i64 val) {
        FTL_ERROR_ON(encode_size(val) > dest.bits, "immediate too large");

        if (val == 0) {
            gen_xor(dest, dest);
            return;
        }

        if (val == 1)
            return;

        if (val == -1) {
            gen_neg(dest);
            return;
        }

        if (is_pow2(val)) {
            gen_shl(dest, log2i(val));
            return;
        }

        value src = m_alloc.new_local(dest.bits, val);
        gen_imul(dest, src);
    }

    void cgen::gen_idiv(value& dest, i64 val) {
        FTL_ERROR_ON(encode_size(val) > dest.bits, "immediate too large");
        FTL_ERROR_ON(val == 0, "division by zero");

        if (val == 1)
            return;

        if (val == -1) {
            gen_neg(dest);
            return;
        }

        if (is_pow2(val)) {
            gen_sha(dest, log2i(val));
            return;
        }

        value src = m_alloc.new_local(dest.bits, val);
        gen_idiv(dest, src);
    }

    void cgen::gen_imod(value& dest, i64 val) {
        FTL_ERROR_ON(encode_size(val) > dest.bits, "immediate too large");
        FTL_ERROR_ON(val == 0, "division by zero");

        if (val == 1 || val == -1) {
            gen_xor(dest, dest);
            return;
        }

        value src = m_alloc.new_local(dest.bits, val);
        gen_imod(dest, src);
    }

    void cgen::gen_umul(value& dest, u64 val) {
        FTL_ERROR_ON(encode_size(val) > dest.bits, "immediate too large");

        if (val == 0) {
            gen_xor(dest, dest);
            return;
        }

        if (val == 1)
            return;

        if (is_pow2(val)) {
            gen_shl(dest, log2i(val));
            return;
        }

        value src = m_alloc.new_local(dest.bits, val);
        gen_umul(dest, src);
    }

    void cgen::gen_udiv(value& dest, u64 val) {
        FTL_ERROR_ON(encode_size(val) > dest.bits, "immediate too large");
        FTL_ERROR_ON(val == 0, "division by zero");

        if (val == 1)
            return;

        if (is_pow2(val)) {
            gen_shr(dest, log2i(val));
            return;
        }

        value src = m_alloc.new_local(dest.bits, val);
        gen_udiv(dest, src);
    }

    void cgen::gen_umod(value& dest, u64 val) {
        FTL_ERROR_ON(encode_size(val) > dest.bits, "immediate too large");
        FTL_ERROR_ON(val == 0, "division by zero");

        if (val == 1) {
            gen_xor(dest, dest);
            return;
        }

        if (is_pow2(val) && fits_i32(val)) {
            gen_and(dest, val - 1);
            return;
        }

        value src = m_alloc.new_local(dest.bits, val);
        gen_umod(dest, src);
    }

    void cgen::gen_not(value& dest) {
        m_emitter.notr(dest.bits, dest);
        dest.mark_dirty();
    }

    void cgen::gen_neg(value& dest) {
        m_emitter.negr(dest.bits, dest);
        dest.mark_dirty();
    }

    void cgen::gen_shl(value& dest, value& src) {
        src.fetch(RCX);
        m_emitter.shlr(dest.bits, dest);
        dest.mark_dirty();
    }

    void cgen::gen_shr(value& dest, value& src) {
        src.fetch(RCX);
        m_emitter.shrr(dest.bits, dest);
        dest.mark_dirty();
    }

    void cgen::gen_sha(value& dest, value& src) {
        src.fetch(RCX);
        m_emitter.sarr(dest.bits, dest);
        dest.mark_dirty();
    }

    void cgen::gen_rol(value& dest, value& src) {
        src.fetch(RCX);
        m_emitter.rolr(dest.bits, dest);
        dest.mark_dirty();
    }

    void cgen::gen_ror(value& dest, value& src) {
        src.fetch(RCX);
        m_emitter.rorr(dest.bits, dest);
        dest.mark_dirty();
    }

    void cgen::gen_shl(value& dest, u8 shift) {
        if (shift == 0)
            return;

        m_emitter.shli(dest.bits, dest, shift);
        dest.mark_dirty();
    }

    void cgen::gen_shr(value& dest, u8 shift) {
        if (shift == 0)
            return;

        m_emitter.shri(dest.bits, dest, shift);
        dest.mark_dirty();
    }

    void cgen::gen_sha(value& dest, u8 shift) {
        if (shift == 0)
            return;

        m_emitter.sari(dest.bits, dest, shift);
        dest.mark_dirty();
    }

    void cgen::gen_rol(value& dest, u8 shift) {
        if (shift == 0)
            return;

        m_emitter.roli(dest.bits, dest, shift);
        dest.mark_dirty();
    }

    void cgen::gen_ror(value& dest, u8 shift) {
        if (shift == 0)
            return;

        m_emitter.rori(dest.bits, dest, shift);
        dest.mark_dirty();
    }

    static const reg caller_saved_regs[] = {
            RAX, RCX, RDX, RSP, RDI, RSI, R8, R9, R10, R11
    };

    value cgen::gen_call(func1* fn) {
        for (reg r : caller_saved_regs)
            m_alloc.flush(r);

        m_emitter.movi(64, argreg(0), (u64)m_alloc.get_base_addr());

        value ret = m_alloc.new_local(64, (i64)fn, RAX);
        m_emitter.call(RAX);

        return ret;
    }

    value cgen::gen_call(func2* fn, value& arg1) {
        m_emitter.movr(64, argreg(1), arg1);
        return gen_call((func1*)fn);
    }

    value cgen::gen_call(func3* fn, value& arg1, value& arg2) {
        m_emitter.movr(64, argreg(2), arg2);
        return gen_call((func2*)fn, arg1);
    }

    value cgen::gen_call(func4* fn, value& arg1, value& arg2, value& arg3) {
        m_emitter.movr(64, argreg(3), arg3);
        return gen_call((func3*)fn, arg1, arg2);
    }

}
