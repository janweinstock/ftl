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

#include "ftl/func.h"

namespace ftl {

    void func::gen_prologue_epilogue() {
        for (reg r : callee_saved_regs)
            m_emitter.push(r);

        i32 frame_size = 64 * sizeof(u64);

        m_emitter.subi(64, m_alloc.STACK_POINTER, frame_size);
        m_emitter.movr(64, m_alloc.BASE_REGISTER, argreg(1));
        m_emitter.jmpr(argreg(0));
        m_buffer.align(4);

        m_buffer.mark_exit();
        m_exit.place(false);

        m_emitter.addi(64, m_alloc.STACK_POINTER, frame_size);
        for (size_t i = FTL_ARRAY_SIZE(callee_saved_regs); i != 0; i--)
            m_emitter.pop(callee_saved_regs[i-1]);

        m_emitter.ret();
        m_buffer.align(4);

        m_code = m_buffer.get_code_ptr();
    }

    func::func(const string& nm, size_t bufsz):
        m_name(nm),
        m_bufptr(new cbuf(bufsz)),
        m_buffer(*m_bufptr),
        m_emitter(m_buffer),
        m_alloc(m_emitter),
        m_head(m_buffer.get_code_entry()),
        m_code(m_buffer.get_code_ptr()),
        m_entry(nm + ".entry", m_buffer, m_alloc, m_buffer.get_code_entry()),
        m_exit(nm + ".exit", m_buffer, m_alloc, m_buffer.get_code_exit()) {
        if (m_buffer.is_empty())
            gen_prologue_epilogue();
    }

    func::func(const string& nm, cbuf& buffer, void* dataptr):
        m_name(nm),
        m_bufptr(nullptr),
        m_buffer(buffer),
        m_emitter(m_buffer),
        m_alloc(m_emitter),
        m_head(m_buffer.get_code_entry()),
        m_code(m_buffer.get_code_ptr()),
        m_entry(nm + ".entry", m_buffer, m_alloc, m_buffer.get_code_entry()),
        m_exit(nm + ".exit", m_buffer, m_alloc, m_buffer.get_code_exit()) {
        if (m_buffer.is_empty())
            gen_prologue_epilogue();
        if (dataptr != nullptr)
            set_data_ptr(dataptr);
    }

    func::func(func&& other):
        m_name(other.m_name),
        m_bufptr(other.m_bufptr),
        m_buffer(other.m_buffer),
        m_emitter(std::move(other.m_emitter)),
        m_alloc(std::move(other.m_alloc)),
        m_head(other.m_head),
        m_code(other.m_code),
        m_entry(std::move(other.m_entry)),
        m_exit(std::move(other.m_exit)) {
        other.m_bufptr = nullptr;
    }

    func::~func() {
        if (m_bufptr)
            delete m_bufptr;
    }

    i64 func::exec() {
        return exec((void*)m_alloc.get_base_addr());
    }

    i64 func::exec(void* data) {
        return invoke(m_buffer, m_code, data);
    }

    void func::set_data_ptr(void* ptr) {
        m_alloc.set_base_addr((u64)ptr);
    }

    void func::set_data_ptr_stack() {
        int dummy;
        set_data_ptr(&dummy);
    }

    void func::set_data_ptr_heap() {
        int* dummy = new int;
        set_data_ptr(dummy);
        delete dummy;
    }

    void func::set_data_ptr_code() {
        set_data_ptr(m_buffer.get_code_ptr());
    }

    void func::gen_ret() {
        m_alloc.flush_all_regs();
        gen_jmp(m_exit, true);
    }

    void func::gen_ret(i64 val) {
        m_alloc.flush_all_regs();
        m_emitter.movi(64, RAX, val);
        gen_ret();
    }

    void func::gen_ret(value& val) {
        m_emitter.movsx(64, val.bits, RAX, val);
        m_alloc.flush_all_regs();
        gen_ret();
    }

    void func::gen_jmp(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_alloc.flush_all_regs();
        m_emitter.jmpi(offset, &fix);
        l.add(fix);
    }

    void func::gen_jo(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_alloc.flush_all_regs();
        m_emitter.jo(offset, &fix);
        l.add(fix);
    }

    void func::gen_jno(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_alloc.flush_all_regs();
        m_emitter.jno(offset, &fix);
        l.add(fix);
    }

    void func::gen_jb(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_alloc.flush_all_regs();
        m_emitter.jb(offset, &fix);
        l.add(fix);
    }

    void func::gen_jae(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_alloc.flush_all_regs();
        m_emitter.jae(offset, &fix);
        l.add(fix);
    }

    void func::gen_jz(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_alloc.flush_all_regs();
        m_emitter.jz(offset, &fix);
        l.add(fix);
    }

    void func::gen_jnz(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_alloc.flush_all_regs();
        m_emitter.jnz(offset, &fix);
        l.add(fix);
    }

    void func::gen_je(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_alloc.flush_all_regs();
        m_emitter.je(offset, &fix);
        l.add(fix);
    }

    void func::gen_jne(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_alloc.flush_all_regs();
        m_emitter.jne(offset, &fix);
        l.add(fix);
    }

    void func::gen_jbe(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_alloc.flush_all_regs();
        m_emitter.jbe(offset, &fix);
        l.add(fix);
    }

    void func::gen_ja(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_alloc.flush_all_regs();
        m_emitter.ja(offset, &fix);
        l.add(fix);
    }

    void func::gen_js(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_alloc.flush_all_regs();
        m_emitter.js(offset, &fix);
        l.add(fix);
    }

    void func::gen_jns(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_alloc.flush_all_regs();
        m_emitter.jns(offset, &fix);
        l.add(fix);
    }

    void func::gen_jp(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_alloc.flush_all_regs();
        m_emitter.jp(offset, &fix);
        l.add(fix);
    }

    void func::gen_jnp(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_alloc.flush_all_regs();
        m_emitter.jnp(offset, &fix);
        l.add(fix);
    }

    void func::gen_jl(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_alloc.flush_all_regs();
        m_emitter.jl(offset, &fix);
        l.add(fix);
    }

    void func::gen_jge(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_alloc.flush_all_regs();
        m_emitter.jge(offset, &fix);
        l.add(fix);
    }

    void func::gen_jle(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_alloc.flush_all_regs();
        m_emitter.jle(offset, &fix);
        l.add(fix);
    }

    void func::gen_jg(label& l, bool far) {
        fixup fix;
        i32 offset = far ? 128 : 0;
        m_alloc.flush_all_regs();
        m_emitter.jg(offset, &fix);
        l.add(fix);
    }

    void func::gen_seto(value& dest) {
        if (dest.is_mem())
            dest.assign();

        m_emitter.seto(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void func::gen_setno(value& dest) {
        if (dest.is_mem())
            dest.assign();

        m_emitter.setno(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void func::gen_setb(value& dest) {
        if (dest.is_mem())
            dest.assign();

        m_emitter.setb(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void func::gen_setae(value& dest) {
        if (dest.is_mem())
            dest.assign();

        m_emitter.setae(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void func::gen_setz(value& dest) {
        if (dest.is_mem())
            dest.assign();

        m_emitter.setz(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void func::gen_setnz(value& dest) {
        if (dest.is_mem())
            dest.assign();

        m_emitter.setnz(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void func::gen_sete(value& dest) {
        if (dest.is_mem())
            dest.assign();

        m_emitter.sete(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void func::gen_setne(value& dest) {
        if (dest.is_mem())
            dest.assign();

        m_emitter.setne(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void func::gen_setbe(value& dest) {
        if (dest.is_mem())
            dest.assign();

        m_emitter.setbe(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void func::gen_seta(value& dest) {
        if (dest.is_mem())
            dest.assign();

        m_emitter.seta(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void func::gen_sets(value& dest) {
        if (dest.is_mem())
            dest.assign();

        m_emitter.sets(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void func::gen_setns(value& dest) {
        if (dest.is_mem())
            dest.assign();

        m_emitter.setns(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void func::gen_setp(value& dest) {
        if (dest.is_mem())
            dest.assign();

        m_emitter.setp(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void func::gen_setnp(value& dest) {
        if (dest.is_mem())
            dest.assign();

        m_emitter.setnp(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void func::gen_setl(value& dest) {
        if (dest.is_mem())
            dest.assign();

        m_emitter.setl(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void func::gen_setge(value& dest) {
        if (dest.is_mem())
            dest.assign();

        m_emitter.setge(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void func::gen_setle(value& dest) {
        if (dest.is_mem())
            dest.assign();

        m_emitter.setle(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void func::gen_setg(value& dest) {
        if (dest.is_mem())
            dest.assign();

        m_emitter.setg(dest);
        m_emitter.movzx(dest.bits, 8, dest, dest);
        dest.mark_dirty();
    }

    void func::gen_cmovo(value& dest, const value& src) {
        if (dest == src)
            return;

        if (dest.is_mem())
            dest.fetch();

        m_emitter.cmovo(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_cmovno(value& dest, const value& src) {
        if (dest == src)
            return;

        if (dest.is_mem())
            dest.fetch();

        m_emitter.cmovno(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_cmovb(value& dest, const value& src) {
        if (dest == src)
            return;

        if (dest.is_mem())
            dest.fetch();

        m_emitter.cmovb(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_cmovae(value& dest, const value& src) {
        if (dest == src)
            return;

        if (dest.is_mem())
            dest.fetch();

        m_emitter.cmovae(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_cmovz(value& dest, const value& src) {
        if (dest == src)
            return;

        if (dest.is_mem())
            dest.fetch();

        m_emitter.cmovz(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_cmovnz(value& dest, const value& src) {
        if (dest == src)
            return;

        if (dest.is_mem())
            dest.fetch();

        m_emitter.cmovnz(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_cmove(value& dest, const value& src) {
        if (dest == src)
            return;

        if (dest.is_mem())
            dest.fetch();

        m_emitter.cmove(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_cmovne(value& dest, const value& src) {
        if (dest == src)
            return;

        if (dest.is_mem())
            dest.fetch();

        m_emitter.cmovne(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_cmovbe(value& dest, const value& src) {
        if (dest == src)
            return;

        if (dest.is_mem())
            dest.fetch();

        m_emitter.cmovbe(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_cmova(value& dest, const value& src) {
        if (dest == src)
            return;

        if (dest.is_mem())
            dest.fetch();

        m_emitter.cmova(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_cmovs(value& dest, const value& src) {
        if (dest == src)
            return;

        if (dest.is_mem())
            dest.fetch();

        m_emitter.cmovs(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_cmovns(value& dest, const value& src) {
        if (dest == src)
            return;

        if (dest.is_mem())
            dest.fetch();

        m_emitter.cmovns(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_cmovp(value& dest, const value& src) {
        if (dest == src)
            return;

        if (dest.is_mem())
            dest.fetch();

        m_emitter.cmovp(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_cmovnp(value& dest, const value& src) {
        if (dest == src)
            return;

        if (dest.is_mem())
            dest.fetch();

        m_emitter.cmovnp(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_cmovl(value& dest, const value& src) {
        if (dest == src)
            return;

        if (dest.is_mem())
            dest.fetch();

        m_emitter.cmovl(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_cmovge(value& dest, const value& src) {
        if (dest == src)
            return;

        if (dest.is_mem())
            dest.fetch();

        m_emitter.cmovge(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_cmovle(value& dest, const value& src) {
        if (dest == src)
            return;

        if (dest.is_mem())
            dest.fetch();

        m_emitter.cmovle(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_cmovg(value& dest, const value& src) {
        if (dest == src)
            return;

        if (dest.is_mem())
            dest.fetch();

        m_emitter.cmovg(dest.bits, dest, src);
        dest.mark_dirty();
    }


    void func::gen_mov(value& dest, const value& src) {
        if (dest == src)
            return;

        if (dest.is_mem() && src.is_mem())
            dest.assign();

        if (dest.bits > src.bits && src.bits < 32)
            m_emitter.movzx(dest.bits, src.bits, dest, src);
        else
            m_emitter.movr(min(dest.bits, src.bits), dest, src);

        dest.mark_dirty();
    }

    void func::gen_add(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            dest.fetch();
        m_emitter.addr(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_or(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            dest.fetch();
        m_emitter.orr(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_adc(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            dest.fetch();
        m_emitter.adcr(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_sbb(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            dest.fetch();
        m_emitter.sbbr(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_and(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            dest.fetch();
        m_emitter.andr(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_sub(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            dest.fetch();
        m_emitter.subr(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_xor(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            dest.fetch();
        m_emitter.xorr(dest.bits, dest, src);
        dest.mark_dirty();
    }

    void func::gen_cmp(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            dest.fetch();
        m_emitter.cmpr(dest.bits, dest, src);
    }

    void func::gen_tst(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            dest.fetch();
        m_emitter.tstr(dest.bits, dest, src);
    }

    void func::gen_xchg(value& dest, value& src) {
        if (dest.is_mem())
            dest.fetch();
        m_emitter.xchg(dest.bits, dest, src);
        dest.mark_dirty();
        src.mark_dirty();
    }

    void func::gen_mov(value& dest, i64 val) {
        int immlen = max(encode_size(val), dest.bits);
        if (dest.is_mem() && immlen > 32)
            dest.assign();

        if (val == 0 && dest.is_reg())
            m_emitter.xorr(dest.bits, dest, dest);
        else
            m_emitter.movi(dest.bits, dest, val);

        dest.mark_dirty();
    }

    void func::gen_add(value& dest, i32 val) {
        switch (val) {
        case  0: return;
        case  1: m_emitter.incr(dest.bits, dest); break;
        case -1: m_emitter.decr(dest.bits, dest); break;
        default: m_emitter.addi(dest.bits, dest, val); break;
        }

        dest.mark_dirty();
    }

    void func::gen_or(value& dest, i32 val) {
        if (val == 0)
            return;

        m_emitter.ori(dest.bits, dest, val);
        dest.mark_dirty();
    }

    void func::gen_adc(value& dest, i32 val) {
        m_emitter.adci(dest.bits, dest, val);
        dest.mark_dirty();
    }

    void func::gen_sbb(value& dest, i32 val) {
        m_emitter.sbbi(dest.bits, dest, val);
        dest.mark_dirty();
    }

    void func::gen_and(value& dest, i32 val) {
        if (val == -1)
            return;

        if (val == 0 && dest.is_reg())
            m_emitter.xorr(dest.bits, dest, dest);
        else
            m_emitter.andi(dest.bits, dest, val);
        dest.mark_dirty();
    }

    void func::gen_sub(value& dest, i32 val) {
        switch (val) {
        case  0: return;
        case  1: m_emitter.decr(dest.bits, dest); break;
        case -1: m_emitter.incr(dest.bits, dest); break;
        default: m_emitter.subi(dest.bits, dest, val); break;
        }

        dest.mark_dirty();
    }

    void func::gen_xor(value& dest, i32 val) {
        m_emitter.xori(dest.bits, dest, val);
        dest.mark_dirty();
    }

    void func::gen_cmp(value& dest, i32 val) {
        m_emitter.cmpi(dest.bits, dest, val);
    }

    void func::gen_tst(value& dest, i32 val) {
        m_emitter.tsti(dest.bits, dest, val);
    }

    void func::gen_imul(value& hi, value& dest, const value& src) {
        m_alloc.fetch(&dest, RAX);
        m_alloc.flush(RDX);
        m_emitter.imul(dest.bits, src);
        m_alloc.assign(&hi, RDX);
        m_alloc.mark_dirty(RAX);
        m_alloc.mark_dirty(RDX);
    }

    void func::gen_umul(value& hi, value& dest, const value& src) {
        m_alloc.fetch(&dest, RAX);
        m_alloc.flush(RDX);
        m_emitter.mulr(dest.bits, src);
        m_alloc.assign(&hi, RDX);
        m_alloc.mark_dirty(RAX);
        m_alloc.mark_dirty(RDX);
    }

    void func::gen_imul(value& dest, const value& src) {
        value dummy = gen_local_val("imul.hi", dest.bits, RDX);
        gen_imul(dummy, dest, src);
        free_value(dummy);
    }

    void func::gen_umul(value& dest, const value& src) {
        value dummy = gen_local_val("umul.hi", dest.bits, RDX);
        gen_umul(dummy, dest, src);
        free_value(dummy);
    }

    void func::gen_idiv(value& dest, const value& src) {
        m_alloc.fetch(&dest, RAX);
        m_alloc.flush(RDX);
        m_emitter.cwd(dest.bits);
        m_emitter.idiv(dest.bits, src);
        m_alloc.mark_dirty(RAX);
    }

    void func::gen_imod(value& dest, const value& src) {
        gen_idiv(dest, src);
        m_alloc.assign(&dest, RDX);
        m_alloc.mark_dirty(RDX);
    }

    void func::gen_udiv(value& dest, const value& src) {
        m_alloc.fetch(&dest, RAX);
        m_alloc.flush(RDX);
        m_emitter.xorr(32, RDX, RDX);
        m_emitter.divr(dest.bits, src);
        dest.mark_dirty();
    }

    void func::gen_umod(value& dest, const value& src) {
        gen_udiv(dest, src);
        dest.assign(RDX);
        dest.mark_dirty();
    }

    void func::gen_imul(value& dest, i64 val) {
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

        value src = gen_local_val("temp_imul_imm", dest.bits, val);
        gen_imul(dest, src);
        m_alloc.free_value(src);
    }

    void func::gen_idiv(value& dest, i64 val) {
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

        value src = gen_local_val("temp_idiv_imm", dest.bits, val);
        gen_idiv(dest, src);
    }

    void func::gen_imod(value& dest, i64 val) {
        FTL_ERROR_ON(encode_size(val) > dest.bits, "immediate too large");
        FTL_ERROR_ON(val == 0, "division by zero");

        if (val == 1 || val == -1) {
            gen_xor(dest, dest);
            return;
        }

        value src = gen_local_val("temp_imod_imm", dest.bits, val);
        gen_imod(dest, src);
    }

    void func::gen_umul(value& dest, u64 val) {
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

        value src = gen_local_val("temp_umul_imm", dest.bits, val);
        gen_umul(dest, src);
        m_alloc.free_value(src);
    }

    void func::gen_udiv(value& dest, u64 val) {
        FTL_ERROR_ON(encode_size(val) > dest.bits, "immediate too large");
        FTL_ERROR_ON(val == 0, "division by zero");

        if (val == 1)
            return;

        if (is_pow2(val)) {
            gen_shr(dest, log2i(val));
            return;
        }

        value src = gen_local_val("temp_udiv_imm", dest.bits, val);
        gen_udiv(dest, src);
    }

    void func::gen_umod(value& dest, u64 val) {
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

        value src = gen_local_val("temp_umod_imm", dest.bits, val);
        gen_umod(dest, src);
    }

    void func::gen_inc(value& dest) {
        m_emitter.incr(dest.bits, dest);
        dest.mark_dirty();
    }

    void func::gen_dec(value& dest) {
        m_emitter.decr(dest.bits, dest);
        dest.mark_dirty();
    }

    void func::gen_not(value& dest) {
        m_emitter.notr(dest.bits, dest);
        dest.mark_dirty();
    }

    void func::gen_neg(value& dest) {
        m_emitter.negr(dest.bits, dest);
        dest.mark_dirty();
    }

    void func::gen_shl(value& dest, value& src) {
        src.fetch(RCX);
        m_emitter.shlr(dest.bits, dest);
        dest.mark_dirty();
    }

    void func::gen_shr(value& dest, value& src) {
        src.fetch(RCX);
        m_emitter.shrr(dest.bits, dest);
        dest.mark_dirty();
    }

    void func::gen_sha(value& dest, value& src) {
        src.fetch(RCX);
        m_emitter.sarr(dest.bits, dest);
        dest.mark_dirty();
    }

    void func::gen_rol(value& dest, value& src) {
        src.fetch(RCX);
        m_emitter.rolr(dest.bits, dest);
        dest.mark_dirty();
    }

    void func::gen_ror(value& dest, value& src) {
        src.fetch(RCX);
        m_emitter.rorr(dest.bits, dest);
        dest.mark_dirty();
    }

    void func::gen_shl(value& dest, u8 shift) {
        if (shift == 0)
            return;

        m_emitter.shli(dest.bits, dest, shift);
        dest.mark_dirty();
    }

    void func::gen_shr(value& dest, u8 shift) {
        if (shift == 0)
            return;

        m_emitter.shri(dest.bits, dest, shift);
        dest.mark_dirty();
    }

    void func::gen_sha(value& dest, u8 shift) {
        if (shift == 0)
            return;

        m_emitter.sari(dest.bits, dest, shift);
        dest.mark_dirty();
    }

    void func::gen_rol(value& dest, u8 shift) {
        if (shift == 0)
            return;

        m_emitter.roli(dest.bits, dest, shift);
        dest.mark_dirty();
    }

    void func::gen_ror(value& dest, u8 shift) {
        if (shift == 0)
            return;

        m_emitter.rori(dest.bits, dest, shift);
        dest.mark_dirty();
    }

    void func::gen_zxt(value& dest, value& src) {
        gen_zxt(dest, src, dest.bits, src.bits);
    }

    void func::gen_zxt(value& dest, value& src, int dbits, int sbits) {
        FTL_ERROR_ON(dbits < sbits, "target width too narrow");

        if (dest != src) {
            dest.assign();
            m_emitter.movzx(dbits, sbits, dest, src);
        } else {
            reg r = m_alloc.select();
            m_alloc.flush(r);
            m_emitter.movzx(dbits, sbits, r, src);
            dest.assign(r);
        }

        dest.mark_dirty();
    }

    void func::gen_sxt(value& dest, value& src) {
        gen_sxt(dest, src, dest.bits, src.bits);
    }

    void func::gen_sxt(value& dest, value& src, int dbits, int sbits) {
        FTL_ERROR_ON(dbits < sbits, "target width too narrow");

        if (dest != src) {
            dest.assign();
            m_emitter.movsx(dbits, sbits, dest, src);
        } else {
            reg r = m_alloc.select();
            m_alloc.flush(r);
            m_emitter.movsx(dbits, sbits, r, src);
            dest.assign(r);
        }

        dest.mark_dirty();
    }

    void func::gen_cmpxchg(value& dest, value& src, value& cmpv) {
        dest.flush();
        src.fetch();
        cmpv.fetch(RAX);

        m_emitter.cmpxchg(dest.bits, dest, src);
    }

    void func::gen_fence(bool sync_loads, bool sync_stores) {
        if (sync_loads && sync_stores)
            m_emitter.mfence();
        else if (sync_loads)
            m_emitter.lfence();
        else if (sync_stores)
            m_emitter.sfence();
    }

    void func::gen_mov(scalar& dest, const value& src) {
        if (dest.is_mem())
            dest.assign();

        m_emitter.movx(dest.bits, dest, src);
    }

    void func::gen_mov(value& dest, const scalar& src) {
        FTL_ERROR_ON(dest.bits < 32, "integer value too narrow");

        if (dest.is_mem())
            dest.assign();

        if (dest.bits == src.bits) {
            m_emitter.movx(dest.bits, dest, src);
            return;
        }

        scalar temp = gen_scratch_fp("temp.movs", dest.bits);
        m_emitter.cvts2s(temp.bits, src.bits, temp, src);
        m_emitter.movx(dest.bits, dest, temp);
    }

    void func::gen_mov(scalar& dest, const scalar& src) {
        if (dest == src)
            return;

        if (dest.is_mem())
            dest.assign();

        if (dest.bits == src.bits)
            m_emitter.movs(dest.bits, dest, src);
        else
            m_emitter.cvts2s(dest.bits, src.bits, dest, src);
    }

    void func::gen_add(scalar& dest, const scalar& src) {
        if (dest.is_mem())
            dest.fetch();

        dest.mark_dirty();

        if (dest.bits == src.bits) {
            m_emitter.adds(dest.bits, dest, src);
            return;
        }

        scalar temp = gen_scratch_fp("add.temp", dest.bits);
        m_emitter.cvts2s(temp.bits, src.bits, temp, src);
        m_emitter.adds(dest.bits, dest, temp);
    }

    void func::gen_sub(scalar& dest, const scalar& src) {
        if (dest.is_mem())
            dest.fetch();

        dest.mark_dirty();

        if (dest.bits == src.bits) {
            m_emitter.subs(dest.bits, dest, src);
            return;
        }

        scalar temp = gen_scratch_fp("sub.temp", dest.bits);
        m_emitter.cvts2s(temp.bits, src.bits, temp, src);
        m_emitter.subs(dest.bits, dest, temp);
    }

    void func::gen_mul(scalar& dest, const scalar& src) {
        if (dest.is_mem())
            dest.fetch();

        dest.mark_dirty();

        if (dest.bits == src.bits) {
            m_emitter.muls(dest.bits, dest, src);
            return;
        }

        scalar temp = gen_scratch_fp("mul.temp", dest.bits);
        m_emitter.cvts2s(temp.bits, src.bits, temp, src);
        m_emitter.muls(dest.bits, dest, temp);
    }

    void func::gen_div(scalar& dest, const scalar& src) {
        if (dest.is_mem())
            dest.fetch();

        dest.mark_dirty();

        if (dest.bits == src.bits) {
            m_emitter.divs(dest.bits, dest, src);
            return;
        }

        scalar temp = gen_scratch_fp("div.temp", dest.bits);
        m_emitter.cvts2s(temp.bits, src.bits, temp, src);
        m_emitter.divs(dest.bits, dest, temp);
    }

    void func::gen_min(scalar& dest, const scalar& src) {
        if (dest.is_mem())
            dest.fetch();

        dest.mark_dirty();

        if (dest.bits == src.bits) {
            m_emitter.mins(dest.bits, dest, src);
            return;
        }

        scalar temp = gen_scratch_fp("min.temp", dest.bits);
        m_emitter.cvts2s(temp.bits, src.bits, temp, src);
        m_emitter.mins(dest.bits, dest, temp);
    }

    void func::gen_max(scalar& dest, const scalar& src) {
        if (dest.is_mem())
            dest.fetch();

        dest.mark_dirty();

        if (dest.bits == src.bits) {
            m_emitter.maxs(dest.bits, dest, src);
            return;
        }

        scalar temp = gen_scratch_fp("max.temp", dest.bits);
        m_emitter.cvts2s(temp.bits, src.bits, temp, src);
        m_emitter.maxs(dest.bits, dest, temp);
    }

    void func::gen_cmp(scalar& op1, const scalar& op2) {
        if (op1.is_mem())
            op1.fetch();

        if (op1.bits == op2.bits) {
            m_emitter.comis(op1.bits, op1, op2);
            return;
        }

        scalar temp = gen_scratch_fp("convert.temp", op1.bits);
        m_emitter.cvts2s(temp.bits, op2.bits, temp, op2);
        m_emitter.comis(op1.bits, op1, temp);
    }

}
