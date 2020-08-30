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
    struct arg_traits;

    template <> struct arg_traits<bool> {
        typedef reg target_register_type;
        static void fetch(alloc& a, unsigned int n, bool val) {
            emitter& e = a.get_emitter();
            reg r = argreg(n + 1);
            a.flush(r);
            e.movi(8, r, val);
        }
    };

    template <> struct arg_traits<i8> {
        typedef reg target_register_type;
        static void fetch(alloc& a, unsigned int n, i8 val) {
            emitter& e = a.get_emitter();
            reg r = argreg(n + 1);
            a.flush(r);
            e.movi(8, r, val);
        }
    };

    template <> struct arg_traits<i16> {
        typedef reg target_register_type;
        static void fetch(alloc& a, unsigned int n, i16 val) {
            emitter& e = a.get_emitter();
            reg r = argreg(n + 1);
            a.flush(r);
            e.movi(16, r, val);
        }
    };

    template <> struct arg_traits<i32> {
        typedef reg target_register_type;
        static void fetch(alloc& a, unsigned int n, i32 val) {
            emitter& e = a.get_emitter();
            reg r = argreg(n + 1);
            a.flush(r);
            e.movi(32, r, val);
        }
    };

    template <> struct arg_traits<i64> {
        typedef reg target_register_type;
        static void fetch(alloc& a, unsigned int n, i64 val) {
            emitter& e = a.get_emitter();
            reg r = argreg(n + 1);
            a.flush(r);
            e.movi(64, r, val);
        }
    };

    template <> struct arg_traits<u8> {
        typedef reg target_register_type;
        static void fetch(alloc& a, unsigned int n, u8 val) {
            emitter& e = a.get_emitter();
            reg r = argreg(n + 1);
            a.flush(r);
            e.movi(8, r, val);
        }
    };

    template <> struct arg_traits<u16> {
        typedef reg target_register_type;
        static void fetch(alloc& a, unsigned int n, u16 val) {
            emitter& e = a.get_emitter();
            reg r = argreg(n + 1);
            a.flush(r);
            e.movi(16, r, val);
        }
    };

    template <> struct arg_traits<u32> {
        typedef reg target_register_type;
        static void fetch(alloc& a, unsigned int n, u32 val) {
            emitter& e = a.get_emitter();
            reg r = argreg(n + 1);
            a.flush(r);
            e.movi(32, r, val);
        }
    };

    template <> struct arg_traits<u64> {
        typedef reg target_register_type;
        static void fetch(alloc& a, unsigned int n, u64 val) {
            emitter& e = a.get_emitter();
            reg r = argreg(n + 1);
            a.flush(r);
            e.movi(64, r, (i64)val);
        }
    };

    template <> struct arg_traits<f32> {
        typedef xmm target_register_type;
        static void fetch(alloc& a, unsigned int n, f32 val) {
            emitter& e = a.get_emitter();
            xmm r = argxmm(n);
            a.flush(r);
            a.flush(RAX);
            e.movi(32, RAX, f32_raw(val));
            e.movs(32, r, RAX);
        }
    };

    template <> struct arg_traits<f64> {
        typedef xmm target_register_type;
        static void fetch(alloc& a, unsigned int n, f64 val) {
            emitter& e = a.get_emitter();
            xmm r = argxmm(n);
            a.flush(r);
            a.flush(RAX);
            e.movi(64, RAX, f64_raw(val));
            e.movs(64, r, RAX);
        }
    };

    template <typename T>
    struct arg_traits<T*> {
        typedef reg target_register_type;
        static void fetch(alloc& a, unsigned int n, T* val) {
            emitter& e = a.get_emitter();
            reg r = argreg(n + 1);
            a.flush(r);
            e.movi(64, r, (uintptr_t)val);
        }
    };

    template <> struct arg_traits<value> {
        typedef reg target_register_type;
        static void fetch(alloc& a, unsigned int n, const value& val) {
            emitter& e = a.get_emitter();
            reg r = argreg(n + 1);
            a.flush(r);
            e.movr(val.bits, r, val);
        }
    };

    template <> struct arg_traits<scalar> {
        typedef xmm target_register_type;
        static void fetch(alloc& a, unsigned int n, const scalar& val) {
            emitter& e = a.get_emitter();
            xmm r = argxmm(n);
            a.flush(r);
            e.movs(val.bits, r, val);
        }
    };

    template <typename T, typename A>
    inline constexpr unsigned int argno() {
        typedef typename arg_traits<T>::target_register_type this_reg_type;
        typedef typename arg_traits<A>::target_register_type other_reg_type;
        return std::is_same<this_reg_type,other_reg_type>() ? 1 : 0;
    }

    template <typename T, typename A, typename B>
    inline constexpr unsigned int argno() {
        return argno<T,A>() + argno<T,B>();
    }

    template <typename T, typename A, typename B, typename C>
    inline constexpr unsigned int argno() {
        return argno<T,A,B>() + argno<T,C>();
    }

    template <typename T, typename A, typename B, typename C, typename D>
    inline constexpr unsigned int argno() {
        return argno<T,A,B,C>() + argno<T,D>();
    }

    template <typename T>
    struct arg {
        static void fetch(alloc& a, const T& val) {
            arg_traits<T>::fetch(a, 0, val);
        }

        template <typename A>
        static void fetch(alloc& a, const T& val) {
            arg_traits<T>::fetch(a, argno<T,A>(), val);
        }

        template <typename A, typename B>
        static void fetch(alloc& a, const T& val) {
            arg_traits<T>::fetch(a, argno<T,A,B>(), val);
        }

        template <typename A, typename B, typename C>
        static void fetch(alloc& a, const T& val) {
            arg_traits<T>::fetch(a, argno<T,A,B,C>(), val);
        }

        template <typename A, typename B, typename C, typename D>
        static void fetch(alloc& a, const T& val) {
            arg_traits<T>::fetch(a, argno<T,A,B,C,D>(), val);
        }
    };

}

#endif
