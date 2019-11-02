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

#ifndef FTL_FUNC_H
#define FTL_FUNC_H

#include "ftl/common.h"
#include "ftl/bitops.h"
#include "ftl/error.h"

#include "ftl/reg.h"
#include "ftl/call.h"
#include "ftl/cbuf.h"
#include "ftl/fixup.h"
#include "ftl/label.h"
#include "ftl/value.h"
#include "ftl/alloc.h"
#include "ftl/emitter.h"

namespace ftl {

    class func
    {
    private:
        string  m_name;

        cbuf*   m_bufptr;
        cbuf&   m_buffer;

        emitter m_emitter;
        alloc   m_alloc;

        u8*     m_head;
        u8*     m_code;

        label   m_entry;
        label   m_exit;

        void gen_prologue_epilogue();

    public:
        const char* name() const { return m_name.c_str(); }

        cbuf&    get_cbuffer()   { return m_buffer; }
        emitter& get_emitter()   { return m_emitter; }
        alloc&   get_alloc()     { return m_alloc; }

        label&   get_prologue()  { return m_entry; }
        label&   get_epilogue()  { return m_exit; }

        static const size_t PLSIZE = 32;

        func(const string& name, size_t bufsz = 4 * KiB);
        func(const string& name, cbuf& buffer, void* dataptr = NULL);
        func(func&& other);
        ~func();

        func() = delete;
        func(const func&) = delete;

        i64 exec();
        i64 exec(void* data);

        i64 operator () ()           { return exec(); }
        i64 operator () (void* data) { return exec(data); }

        void set_data_ptr(void* ptr);
        void set_data_ptr_stack();
        void set_data_ptr_heap();
        void set_data_ptr_code();

        label gen_label(const string& name);

        value gen_local_val(const string& name, int bits, reg r = NREGS);
        value gen_local_i8 (const string& name, reg r = NREGS);
        value gen_local_i16(const string& name, reg r = NREGS);
        value gen_local_i32(const string& name, reg r = NREGS);
        value gen_local_i64(const string& name, reg r = NREGS);

        value gen_local_val(const string& nm, int bits, i64 v, reg r = NREGS);
        value gen_local_i8 (const string& nm, i8  val, reg r = NREGS);
        value gen_local_i16(const string& nm, i16 val, reg r = NREGS);
        value gen_local_i32(const string& nm, i32 val, reg r = NREGS);
        value gen_local_i64(const string& nm, i64 val, reg r = NREGS);

        value gen_global_val(const string& name, int bits, void* addr);
        value gen_global_i8 (const string& name, void* addr);
        value gen_global_i16(const string& name, void* addr);
        value gen_global_i32(const string& name, void* addr);
        value gen_global_i64(const string& name, void* addr);

        void free_value(value& val);

        func gen_function(const string& name);
        void gen_ret();
        void gen_ret(i64 val);
        void gen_ret(value& val);

        void gen_jmp(label& l, bool far = false);
        void gen_jo(label& l, bool far = false);
        void gen_jno(label& l, bool far = false);
        void gen_jb(label& l, bool far = false);
        void gen_jae(label& l, bool far = false);
        void gen_jz(label& l, bool far = false);
        void gen_jnz(label& l, bool far = false);
        void gen_je(label& l, bool far = false);
        void gen_jne(label& l, bool far = false);
        void gen_jbe(label& l, bool far = false);
        void gen_ja(label& l, bool far = false);
        void gen_js(label& l, bool far = false);
        void gen_jns(label& l, bool far = false);
        void gen_jp(label& l, bool far = false);
        void gen_jnp(label& l, bool far = false);
        void gen_jl(label& l, bool far = false);
        void gen_jge(label& l, bool far = false);
        void gen_jle(label& l, bool far = false);
        void gen_jg(label& l, bool far = false);

        void gen_seto(value& dest);
        void gen_setno(value& dest);
        void gen_setb(value& dest);
        void gen_setae(value& dest);
        void gen_setz(value& dest);
        void gen_setnz(value& dest);
        void gen_sete(value& dest);
        void gen_setne(value& dest);
        void gen_setbe(value& dest);
        void gen_seta(value& dest);
        void gen_sets(value& dest);
        void gen_setns(value& dest);
        void gen_setp(value& dest);
        void gen_setnp(value& dest);
        void gen_setl(value& dest);
        void gen_setge(value& dest);
        void gen_setle(value& dest);
        void gen_setg(value& dest);

        void gen_cmovo (value& dest, const value& src);
        void gen_cmovno(value& dest, const value& src);
        void gen_cmovb (value& dest, const value& src);
        void gen_cmovae(value& dest, const value& src);
        void gen_cmovz (value& dest, const value& src);
        void gen_cmovnz(value& dest, const value& src);
        void gen_cmove (value& dest, const value& src);
        void gen_cmovne(value& dest, const value& src);
        void gen_cmovbe(value& dest, const value& src);
        void gen_cmova (value& dest, const value& src);
        void gen_cmovs (value& dest, const value& src);
        void gen_cmovns(value& dest, const value& src);
        void gen_cmovp (value& dest, const value& src);
        void gen_cmovnp(value& dest, const value& src);
        void gen_cmovl (value& dest, const value& src);
        void gen_cmovge(value& dest, const value& src);
        void gen_cmovle(value& dest, const value& src);
        void gen_cmovg (value& dest, const value& src);

        void gen_mov(value& dest, const value& src);
        void gen_add(value& dest, const value& src);
        void gen_or (value& dest, const value& src);
        void gen_adc(value& dest, const value& src);
        void gen_sbb(value& dest, const value& src);
        void gen_and(value& dest, const value& src);
        void gen_sub(value& dest, const value& src);
        void gen_xor(value& dest, const value& src);
        void gen_cmp(value& dest, const value& src);
        void gen_tst(value& dest, const value& src);

        void gen_mov(value& dest, i64 val);
        void gen_add(value& dest, i32 val);
        void gen_or (value& dest, i32 val);
        void gen_adc(value& dest, i32 val);
        void gen_sbb(value& dest, i32 val);
        void gen_and(value& dest, i32 val);
        void gen_sub(value& dest, i32 val);
        void gen_xor(value& dest, i32 val);
        void gen_cmp(value& dest, i32 val);
        void gen_tst(value& dest, i32 val);

        void gen_imul(value& hi, value& lo, const value& src);
        void gen_umul(value& hi, value& lo, const value& src);
        void gen_imul(value& dest, const value& src);
        void gen_umul(value& dest, const value& src);
        void gen_idiv(value& dest, const value& src);
        void gen_udiv(value& dest, const value& src);
        void gen_imod(value& dest, const value& src);
        void gen_umod(value& dest, const value& src);

        void gen_imul(value& dest, i64 val);
        void gen_idiv(value& dest, i64 val);
        void gen_imod(value& dest, i64 val);
        void gen_umul(value& dest, u64 val);
        void gen_udiv(value& dest, u64 val);
        void gen_umod(value& dest, u64 val);

        void gen_inc(value& dest);
        void gen_dec(value& dest);
        void gen_not(value& dest);
        void gen_neg(value& dest);

        void gen_shl(value& dest, value& src);
        void gen_shr(value& dest, value& src);
        void gen_sha(value& dest, value& src);
        void gen_rol(value& dest, value& src);
        void gen_ror(value& dest, value& src);

        void gen_shl(value& dest, u8 shift);
        void gen_shr(value& dest, u8 shift);
        void gen_sha(value& dest, u8 shift);
        void gen_rol(value& dest, u8 shift);
        void gen_ror(value& dest, u8 shift);

        void gen_fence(bool sync_loads = true, bool sync_stores = true);

        typedef i64 (func1)(void* bptr);
        typedef i64 (func2)(void* bptr, i64 arg1);
        typedef i64 (func3)(void* bptr, i64 arg1, i64 arg2);
        typedef i64 (func4)(void* bptr, i64 arg1, i64 arg2, i64 arg3);

        value gen_call(func1* fn);

        template <typename T1>
        value gen_call(func2* fn, const T1& a);

        template <typename T1, typename T2>
        value gen_call(func3* fn, const T1& a, const T2& b);

        template <typename T1, typename T2, typename T3>
        value gen_call(func4* fn, const T1& a, const T2& b, const T3& c);
    };

    inline label func::gen_label(const string& name) {
        return label(name, m_buffer, m_alloc);
    }

    inline value func::gen_local_val(const string& name, int bits, reg r) {
        return m_alloc.new_local_noinit(name, bits, true, r);
    }

    inline value func::gen_local_i8 (const string& name, reg r) {
        return gen_local_val(name, 8, r);
    }

    inline value func::gen_local_i16(const string& name, reg r) {
        return gen_local_val(name, 16, r);
    }

    inline value func::gen_local_i32(const string& name, reg r) {
        return gen_local_val(name, 32, r);
    }

    inline value func::gen_local_i64(const string& name, reg r) {
        return gen_local_val(name, 64, r);
    }

    inline value func::gen_local_val(const string& nm, int w, i64 v, reg r) {
        FTL_ERROR_ON(encode_size(v) > w, "initialization value for too large");
        return m_alloc.new_local(nm, w, true, v, r);
    }

    inline value func::gen_local_i8(const string& name, i8 val, reg r) {
        return gen_local_val(name, 8, val, r);
    }

    inline value func::gen_local_i16(const string& name, i16 val, reg r) {
        return gen_local_val(name, 16, val, r);
    }

    inline value func::gen_local_i32(const string& name, i32 val, reg r) {
        return gen_local_val(name, 32, val, r);
    }

    inline value func::gen_local_i64(const string& name, i64 val, reg r) {
        return gen_local_val(name, 64, val, r);
    }

    inline value func::gen_global_val(const string& nm, int bits, void* addr) {
        return m_alloc.new_global(nm, bits, true, (u64)addr);
    }

    inline value func::gen_global_i8(const string& name, void* addr) {
        return gen_global_val(name, 8, addr);
    }

    inline value func::gen_global_i16(const string& name, void* addr) {
        return gen_global_val(name, 16, addr);
    }

    inline value func::gen_global_i32(const string& name, void* addr) {
        return gen_global_val(name, 32, addr);
    }

    inline value func::gen_global_i64(const string& name, void* addr) {
        return gen_global_val(name, 64, addr);
    }

    inline void func::free_value(value& val) {
        m_alloc.free_value(val);
    }

    template <typename T1>
    inline value func::gen_call(func2* fn, const T1& a) {
        reg r = argreg(1);
        m_alloc.flush(r);
        arg<T1>::fetch(m_emitter, r, a);
        return gen_call((func1*)fn);
    }

    template <typename T1, typename T2>
    inline value func::gen_call(func3* fn, const T1& a, const T2& b) {
        reg r = argreg(2);
        m_alloc.flush(r);
        arg<T2>::fetch(m_emitter, r, b);
        return gen_call((func2*)fn, a);
    }

    template <typename T1, typename T2, typename T3>
    inline value func::gen_call(func4* fn, const T1& a, const T2& b,
                                const T3& c) {
        reg r = argreg(3);
        m_alloc.flush(r);
        arg<T3>::fetch(m_emitter, r, c);
        return gen_call((func3*)fn, a, b);
    }

    static inline i64 invoke(const cbuf& buffer, void* code, void* data) {
        typedef i64 func_t (void* code, void* data);
        func_t* fn = (func_t*)buffer.get_code_entry();
        return fn(code, data);
    }

}

#endif
