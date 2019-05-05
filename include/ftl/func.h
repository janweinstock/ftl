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
#include "ftl/error.h"
#include "ftl/cbuf.h"

namespace ftl {

    class func
    {
    private:
        typedef int func_t (u8*);
        u8* m_entry;
        u8* m_code;

        // disabled
        func();

    public:
        func(cbuf& c);

        int operator () ();
    };

    inline func::func(cbuf& buffer):
        m_entry(buffer.get_code_entry()),
        m_code(buffer.get_code_ptr()) {
    }

    inline int func::operator () () {
        func_t* fn = (func_t*)m_entry;
        return fn(m_code);
    }

}

#endif
