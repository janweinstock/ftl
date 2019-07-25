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

#ifndef FTL_CALL_H
#define FTL_CALL_H

#include "ftl/common.h"
#include "ftl/bitops.h"
#include "ftl/error.h"

#include "ftl/reg.h"
#include "ftl/value.h"
#include "ftl/alloc.h"
#include "ftl/emitter.h"

namespace ftl {

    template <typename T>
    class arg
    {
    public:
        static void fetch(emitter& e, reg r, const T& val);
    };

    template <>
    inline void arg<i8>::fetch(emitter& e, reg r, const i8& val) {
        e.movi(64, r, (i64)val);
    }

    template <>
    inline void arg<i16>::fetch(emitter& e, reg r, const i16& val) {
        e.movi(64, r, (i64)val);
    }

    template <>
    inline void arg<i32>::fetch(emitter& e, reg r, const i32& val) {
        e.movi(64, r, (i64)val);
    }

    template <>
    inline void arg<i64>::fetch(emitter& e, reg r, const i64& val) {
        e.movi(64, r, val);
    }

    template <>
    inline void arg<u8>::fetch(emitter& e, reg r, const u8& val) {
        e.movi(64, r, (i64)val);
    }

    template <>
    inline void arg<u16>::fetch(emitter& e, reg r, const u16& val) {
        e.movi(64, r, (i64)val);
    }

    template <>
    inline void arg<u32>::fetch(emitter& e, reg r, const u32& val) {
        e.movi(64, r, (i64)val);
    }

    template <>
    inline void arg<u64>::fetch(emitter& e, reg r, const u64& val) {
        e.movi(64, r, (i64)val);
    }

    template <>
    inline void arg<value>::fetch(emitter& e, reg r, const value& val) {
        e.movr(val.bits, r, val);
    }

}

#endif
