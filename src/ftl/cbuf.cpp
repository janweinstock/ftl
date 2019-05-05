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

    u8* cbuf::align(size_t boundary) {
        size_t remain = size_remaining();
        void* ptr = m_code_ptr;

        FTL_ERROR_ON(!std::align(boundary, 1, ptr, remain), "out of memory");

        // fill the padding with NOPs to help disassemblers
        memset(m_code_ptr, 0x90, (u8*)ptr - m_code_ptr);
        return m_code_ptr = (u8*)ptr;
    }

    cbuf::cbuf(size_t size):
        m_size(size),
        m_code_head(NULL),
        m_code_ptr(NULL),
        m_code_end(NULL) {
        int prot = PROT_READ | PROT_WRITE | PROT_EXEC;
        int flags = MAP_PRIVATE | MAP_ANONYMOUS;

        m_code_ptr = m_code_head = (u8*)mmap(NULL, size, prot, flags, -1, 0);
        m_code_end = m_code_head + m_size;

        FTL_ERROR_ON(m_code_head == MAP_FAILED, "mmap: %s", strerror(errno));
    }

    cbuf::~cbuf() {
        if (m_code_head) {
            munmap(m_code_head, m_size);
        }
    }

}
