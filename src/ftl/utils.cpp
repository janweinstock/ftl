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

#include "ftl/utils.h"

namespace ftl {

    string mkstr(const char* format, ...) {
        va_list args;
        va_start(args, format);
        string str = vmkstr(format, args);
        va_end(args);
        return str;
    }

    string vmkstr(const char* format, va_list args) {
        va_list args2;
        va_copy(args2, args);

        int size = vsnprintf(NULL, 0, format, args) + 1;
        if (size <= 0)
            return "";

        char* buffer = new char [size];
        vsnprintf(buffer, size, format, args2);
        va_end(args2);

        string s(buffer);
        delete [] buffer;
        return s;
    }

}
