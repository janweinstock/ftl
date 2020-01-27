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

    class arg
    {
    public:
        template <typename T>
        static void fetch(emitter& e, reg r, T* ptr);

        static void fetch(emitter& e, reg r, i8  val);
        static void fetch(emitter& e, reg r, i16 val);
        static void fetch(emitter& e, reg r, i32 val);
        static void fetch(emitter& e, reg r, i64 val);

        static void fetch(emitter& e, reg r, u8  val);
        static void fetch(emitter& e, reg r, u16 val);
        static void fetch(emitter& e, reg r, u32 val);
        static void fetch(emitter& e, reg r, u64 val);

        static void fetch(emitter& e, reg r, const value& val);
    };

    template <typename T>
    inline void arg::fetch(emitter& e, reg r, T* ptr) {
        arg::fetch(e, r, reinterpret_cast<uintptr_t>(ptr));
    }

    inline void arg::fetch(emitter& e, reg r, i8 val) {
        e.movi(64, r, (i64)val);
    }

    inline void arg::fetch(emitter& e, reg r, i16 val) {
        e.movi(64, r, (i64)val);
    }

    inline void arg::fetch(emitter& e, reg r, i32 val) {
        e.movi(64, r, (i64)val);
    }

    inline void arg::fetch(emitter& e, reg r, i64 val) {
        e.movi(64, r, val);
    }

    inline void arg::fetch(emitter& e, reg r, u8 val) {
        e.movi(64, r, (i64)val);
    }

    inline void arg::fetch(emitter& e, reg r, u16 val) {
        e.movi(64, r, (i64)val);
    }

    inline void arg::fetch(emitter& e, reg r, u32 val) {
        e.movi(64, r, (i64)val);
    }

    inline void arg::fetch(emitter& e, reg r, u64 val) {
        e.movi(64, r, (i64)val);
    }

    inline void arg::fetch(emitter& e, reg r, const value& val) {
        e.movr(val.bits, r, val);
    }

}

#endif
