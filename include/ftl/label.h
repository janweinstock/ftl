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

#ifndef FTL_LABEL_H
#define FTL_LABEL_H

#include "ftl/common.h"
#include "ftl/bitops.h"
#include "ftl/error.h"

#include "ftl/fixup.h"
#include "ftl/cache.h"

namespace ftl {

    class label
    {
    private:
        u8* m_location;
        vector<fixup> m_fixups;
        cache& m_code;

    public:
        label(cache& c);
        ~label();

        fixup* mkfixup();

        void place();
        void finalize();
    };

    inline void label::place() {
        FTL_ERROR_ON(m_location, "label has already been placed");
        m_location = m_code.get_code_ptr();
    }

}


#endif
