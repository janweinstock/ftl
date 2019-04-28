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

#ifndef FTL_CODE_H
#define FTL_CODE_H

#include "ftl/common.h"
#include "ftl/bitops.h"
#include "ftl/error.h"

#include "ftl/cache.h"
#include "ftl/fixup.h"
#include "ftl/label.h"
#include "ftl/emitter.h"

namespace ftl {

    class code
    {
    private:
        cache m_cache;
        emitter m_emitter;

        vector<label> m_labels;

        // disabled
        code();
        code(const code&);

    public:
        code(size_t size);
        virtual ~code();

        label& mklabel();
        void finalize();

        void jmp(label& l, bool far = false);
        void jo(label& l, bool far = false);
        void jno(label& l, bool far = false);
        void jb(label& l, bool far = false);
        void jae(label& l, bool far = false);
        void jz(label& l, bool far = false);
        void jnz(label& l, bool far = false);
        void je(label& l, bool far = false);
        void jne(label& l, bool far = false);
        void jbe(label& l, bool far = false);
        void ja(label& l, bool far = false);
        void js(label& l, bool far = false);
        void jns(label& l, bool far = false);
        void jp(label& l, bool far = false);
        void jnp(label& l, bool far = false);
        void jl(label& l, bool far = false);
        void jge(label& l, bool far = false);
        void jle(label& l, bool far = false);
        void jg(label& l, bool far = false);
    };

}

#endif
