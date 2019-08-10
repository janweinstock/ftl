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

#ifndef FTL_CACHE_H
#define FTL_CACHE_H

#include "ftl/common.h"
#include "ftl/error.h"

namespace ftl {

    class cbuf
    {
    private:
        size_t m_size;

        u8* m_code_head;
        u8* m_code_ptr;
        u8* m_code_end;

        size_t write(const void* ptr, size_t sz);

    public:
        const u8* get_code_entry() const { return m_code_head; }
        const u8* get_code_ptr()   const { return m_code_ptr; }

        u8* get_code_entry() { return m_code_head; }
        u8* get_code_ptr()   { return m_code_ptr; }

        size_t size_remaining() const { return m_code_end - m_code_ptr; }

        u8* align(size_t boundary);

        cbuf(size_t size);
        virtual ~cbuf();

        cbuf() = delete;
        cbuf(const cbuf&) = delete;

        void reset(u8* addr);
        void reset();

        template <typename T>
        size_t write(const T& val);
    };

    inline size_t cbuf::write(const void* ptr, size_t sz) {
        FTL_ERROR_ON(m_code_ptr + sz > m_code_end, "out of code memory");
        memcpy(m_code_ptr, ptr, sz);
        m_code_ptr += sz;
        return sz;
    }

    template <typename T>
    inline size_t cbuf::write(const T& val) {
        return write(&val, sizeof(T));
    }

}

#endif
