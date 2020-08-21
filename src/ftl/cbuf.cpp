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

#include "ftl/cbuf.h"

namespace ftl {

    const char* out_of_memory::what() const noexcept {
        return "ftl::out_of_memory";
    }

    //static const u8 NOP = 0x90;
    static const u8 ILL = 0x06;

    u8* cbuf::mark_exit() {
        FTL_ERROR_ON(m_code_exit, "code exit already marked");
        m_code_exit = m_code_ptr;
        return m_code_exit;
    }

    u8* cbuf::align(size_t alignment) {
        if (alignment == 0)
            return m_code_ptr;

        const size_t mask = (1ull << alignment) - 1;
        const u8* ptr = (u8*)((u64)(m_code_ptr + mask) & ~mask);
        const size_t count = ptr - m_code_ptr;

        skip(count);

        FTL_ERROR_ON(m_code_ptr != ptr, "failed to fill alignment");
        return m_code_ptr;
    }

    cbuf::cbuf(size_t cap):
        m_capacity(cap),
        m_code_head(nullptr),
        m_code_exit(nullptr),
        m_code_ptr(nullptr),
        m_code_end(nullptr) {
        int prot = PROT_READ | PROT_WRITE | PROT_EXEC;
        int flags = MAP_PRIVATE | MAP_ANONYMOUS;

        m_code_ptr = m_code_head = (u8*)mmap(NULL, cap, prot, flags, -1, 0);
        m_code_end = m_code_head + m_capacity;

        memset(m_code_head, ILL, m_capacity);

        FTL_ERROR_ON(m_code_head == MAP_FAILED, "mmap: %s", strerror(errno));
    }

    cbuf::~cbuf() {
        if (m_code_head) {
            munmap(m_code_head, m_capacity);
        }
    }

    void cbuf::skip(size_t count) {
        for (size_t i = 0; i < count; i++)
            write(ILL);
    }

    void cbuf::reset(u8* addr) {
        if (addr < m_code_head || addr >= m_code_end)
            FTL_ERROR("attempt to reset code pointer to outside code memory");

        if (addr > m_code_ptr) {
            memset(m_code_ptr, ILL, addr -  m_code_ptr);
            m_code_ptr = addr;
        } else if (addr < m_code_ptr) {
            memset(addr, ILL, m_code_ptr - addr);
            m_code_ptr = addr;
        }

        if (m_code_ptr < m_code_exit)
            m_code_exit = nullptr;
    }

    void cbuf::reset() {
        reset(m_code_head);
    }

}
