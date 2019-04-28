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

#include "ftl/label.h"

namespace ftl {

    label::label(cache& c):
        m_location(NULL),
        m_fixups(),
        m_code(c) {
    }

    label::~label() {
        // nothing to do
    }

    fixup* label::mkfixup() {
        m_fixups.push_back(fixup());
        return &m_fixups.back();
    }

    void label::finalize() {
        FTL_ERROR_ON(!m_location, "label has not been placed");
        for (auto fix : m_fixups)
            patch_jump(fix, m_location);
        m_fixups.clear();
    }

}
