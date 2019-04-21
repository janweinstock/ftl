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

#ifndef FTL_ERROR_H
#define FTL_ERROR_H

#include "ftl/common.h"
#include "ftl/utils.h"

namespace ftl {

    class error: public std::exception
    {
    private:
        string m_desc;
        string m_file;
        int    m_line;
        string m_what;

        // disabled
        error();

    public:
        const char* desc() const { return m_desc.c_str(); }
        const char* file() const { return m_file.c_str(); }
        int         line() const { return m_line; }

        error(const string& desc, const char* file, int line);
        error(const error& e);
        virtual ~error() throw();

        virtual const char* what() const throw();
    };

}

std::ostream& operator << (std::ostream& os, const ftl::error& err);

#define FTL_ERROR(...)                                                        \
    throw ::ftl::error(::ftl::mkstr(__VA_ARGS__), __FILE__, __LINE__)

#define FTL_ERROR_ON(cond, ...)                                               \
    do {                                                                      \
        if (cond) {                                                           \
            FTL_ERROR(__VA_ARGS__);                                           \
        }                                                                     \
    } while (0)


#endif
