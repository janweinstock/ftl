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

#include "ftl/code.h"

namespace ftl {

    code::code(size_t size):
        m_cache(size),
        m_emitter(m_cache),
        m_alloc(m_emitter),
        m_labels() {
    }

    code::~code() {
        // nothing to do
    }

    label& code::gen_label() {
        m_labels.push_back(label(m_cache));
        return m_labels.back();
    }


    void code::finalize() {
        for (auto label : m_labels)
            label.finalize();
    }

    void code::gen_jmp(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jmpi(offset, l.mkfixup());
    }

    void code::gen_jo(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jo(offset, l.mkfixup());
    }

    void code::gen_jno(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jno(offset, l.mkfixup());
    }

    void code::gen_jb(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jb(offset, l.mkfixup());
    }

    void code::gen_jae(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jae(offset, l.mkfixup());
    }

    void code::gen_jz(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jz(offset, l.mkfixup());
    }

    void code::gen_jnz(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jnz(offset, l.mkfixup());
    }

    void code::gen_je(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.je(offset, l.mkfixup());
    }

    void code::gen_jne(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jne(offset, l.mkfixup());
    }

    void code::gen_jbe(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jbe(offset, l.mkfixup());
    }

    void code::gen_ja(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.ja(offset, l.mkfixup());
    }

    void code::gen_js(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.js(offset, l.mkfixup());
    }

    void code::gen_jns(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jns(offset, l.mkfixup());
    }

    void code::gen_jp(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jp(offset, l.mkfixup());
    }

    void code::gen_jnp(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jnp(offset, l.mkfixup());
    }

    void code::gen_jl(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jl(offset, l.mkfixup());
    }

    void code::gen_jge(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jge(offset, l.mkfixup());
    }

    void code::gen_jle(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jle(offset, l.mkfixup());
    }

    void code::gen_jg(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jg(offset, l.mkfixup());
    }

    void code::gen_add(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            m_alloc.fetch(dest);
        m_emitter.addr(dest.bits, dest, src);
        dest.set_dirty();
    }

    void code::gen_or(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            m_alloc.fetch(dest);
        m_emitter.orr(dest.bits, dest, src);
        dest.set_dirty();
    }

    void code::gen_adc(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            m_alloc.fetch(dest);
        m_emitter.adcr(dest.bits, dest, src);
        dest.set_dirty();
    }

    void code::gen_sbb(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            m_alloc.fetch(dest);
        m_emitter.sbbr(dest.bits, dest, src);
        dest.set_dirty();
    }

    void code::gen_and(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            m_alloc.fetch(dest);
        m_emitter.andr(dest.bits, dest, src);
        dest.set_dirty();
    }

    void code::gen_sub(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            m_alloc.fetch(dest);
        m_emitter.subr(dest.bits, dest, src);
        dest.set_dirty();
    }

    void code::gen_xor(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            m_alloc.fetch(dest);
        m_emitter.addr(dest.bits, dest, src);
        dest.set_dirty();
    }

    void code::gen_cmp(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            m_alloc.fetch(dest);
        m_emitter.cmpr(dest.bits, dest, src);
    }

    void code::gen_tst(value& dest, const value& src) {
        if (dest.is_mem() && src.is_mem())
            m_alloc.fetch(dest);
        m_emitter.tstr(dest.bits, dest, src);
    }

    void code::gen_add(value& dest, i32 val) {
        m_emitter.addi(dest.bits, dest, val);
        dest.set_dirty();
    }

    void code::gen_or(value& dest, i32 val) {
        m_emitter.ori(dest.bits, dest, val);
        dest.set_dirty();
    }

    void code::gen_adc(value& dest, i32 val) {
        m_emitter.adci(dest.bits, dest, val);
        dest.set_dirty();
    }

    void code::gen_sbb(value& dest, i32 val) {
        m_emitter.sbbi(dest.bits, dest, val);
        dest.set_dirty();
    }

    void code::gen_and(value& dest, i32 val) {
        m_emitter.andi(dest.bits, dest, val);
        dest.set_dirty();
    }

    void code::gen_sub(value& dest, i32 val) {
        m_emitter.subi(dest.bits, dest, val);
        dest.set_dirty();
    }

    void code::gen_xor(value& dest, i32 val) {
        m_emitter.xori(dest.bits, dest, val);
        dest.set_dirty();
    }

    void code::gen_cmp(value& dest, i32 val) {
        m_emitter.cmpi(dest.bits, dest, val);
        dest.set_dirty();
    }

    void code::gen_tst(value& dest, i32 val) {
        m_emitter.tsti(dest.bits, dest, val);
        dest.set_dirty();
    }

}
