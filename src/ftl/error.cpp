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

#include "ftl/error.h"

namespace ftl {

    error::error(const string& desc, const char* file, int line):
        std::exception(),
        m_desc(desc),
        m_file(file),
        m_line(line),
        m_what("error") {
        stringstream ss; ss << *this;
        m_what = ss.str();
    }

    error::~error() {
        // nothing to do
    }

    const char* error::what() const throw () {
        return m_what.c_str();
    }

}

std::ostream& operator << (std::ostream& os, const ftl::error& err) {
    os << err.file() << ":" << err.line() << " " << err.desc();
    return os;
}

