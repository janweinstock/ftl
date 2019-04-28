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
        m_labels() {
    }

    code::~code() {
        // nothing to do
    }

    label& code::mklabel() {
        m_labels.push_back(label(m_cache));
        return m_labels.back();
    }

    void code::finalize() {
        for (auto label : m_labels)
            label.finalize();
    }

    void code::jmp(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jmpi(offset, l.mkfixup());
    }

    void code::jo(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jo(offset, l.mkfixup());
    }

    void code::jno(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jno(offset, l.mkfixup());
    }

    void code::jb(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jb(offset, l.mkfixup());
    }

    void code::jae(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jae(offset, l.mkfixup());
    }

    void code::jz(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jz(offset, l.mkfixup());
    }

    void code::jnz(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jnz(offset, l.mkfixup());
    }

    void code::je(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.je(offset, l.mkfixup());
    }

    void code::jne(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jne(offset, l.mkfixup());
    }

    void code::jbe(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jbe(offset, l.mkfixup());
    }

    void code::ja(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.ja(offset, l.mkfixup());
    }

    void code::js(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.js(offset, l.mkfixup());
    }

    void code::jns(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jns(offset, l.mkfixup());
    }

    void code::jp(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jp(offset, l.mkfixup());
    }

    void code::jnp(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jnp(offset, l.mkfixup());
    }

    void code::jl(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jl(offset, l.mkfixup());
    }

    void code::jge(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jge(offset, l.mkfixup());
    }

    void code::jle(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jle(offset, l.mkfixup());
    }

    void code::jg(label& l, bool far) {
        i32 offset = far ? 128 : 0;
        m_emitter.jg(offset, l.mkfixup());
    }

}
