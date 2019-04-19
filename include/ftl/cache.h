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

    class cache
    {
    private:
        size_t m_size;

        u8* m_code_head;
        u8* m_code_ptr;
        u8* m_code_end;

        void write(void* ptr, size_t sz);

        // disabled
        cache();
        cache(const cache&);

    public:
        cache(size_t size);
        virtual ~cache();

        template <typename T>
        void write(const T& val);
    };

    inline void cache::write(void* ptr, size_t sz) {
        FTL_ERROR_ON(m_code_ptr + sz > m_code_end, "out of code memory");
        memcpy(m_code_ptr, ptr, sz);
        m_code_ptr += sz;
    }

    template <typename T>
    inline void cache::write(const T& val) {
        write(&val, sizeof(T));
    }

}

#endif
