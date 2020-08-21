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

#ifndef FTL_FIXUP_H
#define FTL_FIXUP_H

#include "ftl/common.h"
#include "ftl/bitops.h"
#include "ftl/error.h"

namespace ftl {

    struct fixup {
        u8* code;
        int size;
    };

    static inline void patch_jump(const fixup& fix, const u8* target) {
        ptrdiff_t offset = target - fix.code - fix.size;
        int offlen = encode_size(offset) / 8;
        FTL_ERROR_ON(offlen > fix.size, "jump target too far to encode");
        memcpy(fix.code, &offset, fix.size);
    }

    static inline void patch_call(const fixup& fix, const u8* target) {
        patch_jump(fix, target);
    }

    template <typename FUNC>
    static inline bool can_call_directly(u8* origin, FUNC* target) {
        i64 offset = (u8*)target - origin - 5;
        return fits_i32(offset);
    }

}

#endif
