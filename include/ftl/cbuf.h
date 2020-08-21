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

    class out_of_memory: public std::exception
    {
    public:
        out_of_memory() noexcept {};
        virtual ~out_of_memory() noexcept {}
        virtual const char* what() const noexcept;
    };

    class cbuf
    {
    private:
        size_t m_capacity;

        u8* m_code_head;
        u8* m_code_exit;
        u8* m_code_ptr;
        u8* m_code_end;

        size_t write(const void* ptr, size_t sz);

    public:
        const u8* get_code_entry() const { return m_code_head; }
        const u8* get_code_exit()  const { return m_code_exit; }
        const u8* get_code_ptr()   const { return m_code_ptr; }

        u8* get_code_entry() { return m_code_head; }
        u8* get_code_exit()  { return m_code_exit; }
        u8* get_code_ptr()   { return m_code_ptr; }

        size_t size() const { return m_code_ptr - m_code_head; }
        size_t size_remaining() const { return m_code_end - m_code_ptr; }
        size_t capacity() const { return m_capacity; }

        bool is_empty() const { return m_code_ptr == m_code_head; }
        bool is_full() const { return m_code_ptr >= m_code_end; }

        u8* mark_exit();
        u8* align(size_t alignment);

        cbuf(size_t capacity);
        virtual ~cbuf();

        cbuf() = delete;
        cbuf(const cbuf&) = delete;

        void skip(size_t count);

        void reset(u8* addr);
        void reset();

        template <typename T>
        size_t write(const T& val);
    };

    inline size_t cbuf::write(const void* ptr, size_t sz) {
        size_t n = min(sz, size_remaining());
        memcpy(m_code_ptr, ptr, n);
        m_code_ptr += n;
        if (n != sz)
            throw out_of_memory();
        return n;
    }

    template <typename T>
    inline size_t cbuf::write(const T& val) {
        return write(&val, sizeof(T));
    }

}

#endif
