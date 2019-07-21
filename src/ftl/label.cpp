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

    void label::patch() {
        if (!is_placed())
            FTL_ERROR("cannot patch: label '%s' not yet placed", name());

        for (auto fix : m_fixups)
            patch_jump(fix, m_location);
        m_fixups.clear();
    }


    label::label(const string& name, cbuf& buffer):
        m_location(NULL),
        m_fixups(),
        m_buffer(buffer),
        m_name(name) {
    }

    label::label(label&& other):
        m_location(other.m_location),
        m_fixups(other.m_fixups),
        m_buffer(other.m_buffer),
        m_name(other.m_name) {
    }

    label::~label() {
        if (!is_placed() && !m_fixups.empty())
            FTL_ERROR("unplaced label '%s'", name());
    }

    void label::add(const fixup& fix) {
        m_fixups.push_back(fix);
        if (is_placed())
            patch();
    }

    void label::place() {
        FTL_ERROR_ON(m_location, "label '%s' has already been placed", name());
        m_location = m_buffer.get_code_ptr();
        patch();
    }

}
