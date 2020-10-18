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

#ifndef FTL_REG_H
#define FTL_REG_H

#include "ftl/common.h"
#include "ftl/bitops.h"
#include "ftl/error.h"

namespace ftl {

    enum reg {
        RAX   = 0,  // caller-saved, return value
        RCX   = 1,  // caller-saved, arg3
        RDX   = 2,  // caller-saved, arg2
        RBX   = 3,
        RSP   = 4,
        RBP   = 5,
        RSI   = 6,  // arg1
        RDI   = 7,  // arg0
        R8    = 8,  // caller-saved, arg4
        R9    = 9,  // caller-saved, arg5
        R10   = 10, // caller-saved
        R11   = 11, // caller-saved
        R12   = 12,
        R13   = 13,
        R14   = 14,
        R15   = 15,
        NREGS = 16,

        STACK_POINTER = RSP, // base address register for locals
        BASE_POINTER  = RBP, // base address register for globals
    };

    static inline bool reg_valid(int r) {
        return r < NREGS;
    }

    const array<reg, NREGS> all_regs = {
        RAX, RCX, RDX, RBX, RSP, RBP, RSI, RDI,
        R8,  R9,  R10, R11, R12, R13, R14, R15,
    };

    const array<const char*, NREGS> reg_names = {
        "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi",
        "r8",  "r9",  "r10", "r11", "r12", "r13", "r14", "r15"
    };

#ifdef linux
    const array<reg, 6> param_regs = {
        RDI, RSI, RDX, RCX, R8, R9,
    };

    const array<reg, 7> callee_saved_regs = {
        RBX, RSP, RBP, R12, R13, R14, R15,
    };

    const array<reg, 9> caller_saved_regs = {
        RAX, RCX, RDX, RSI, RDI, R8, R9, R10, R11,
    };
#endif

    static inline reg argreg(unsigned int argno) {
        FTL_ERROR_ON(argno >= param_regs.size(), "argno out of bounds");
        return param_regs[argno];
    }

    enum xmm {
        XMM0  = 0,
        XMM1  = 1,
        XMM2  = 2,
        XMM3  = 3,
        XMM4  = 4,
        XMM5  = 5,
        XMM6  = 6,
        XMM7  = 7,
        XMM8  = 8,
        XMM9  = 9,
        XMM10 = 10,
        XMM11 = 11,
        XMM12 = 12,
        XMM13 = 13,
        XMM14 = 14,
        XMM15 = 15,
        NXMM  = 16,
    };

    static inline bool xmm_valid(int r) {
        return r < NXMM;
    }

    const array<xmm, NXMM> all_xmms = {
        XMM0, XMM1, XMM2,  XMM3,  XMM4,  XMM5,  XMM6,  XMM7,
        XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15,
    };

    const array<const char*, NREGS> xmm_names = {
        "xmm0", "xmm1", "xmm2",  "xmm3",  "xmm4",  "xmm5",  "xmm6",  "xmm7",
        "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15",
    };

#ifdef linux
    const array<xmm, 8> param_xmms = {
        XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7,
    };

    const array<xmm, 0> callee_saved_xmms = { };

    const array<xmm, NXMM> caller_saved_xmms = {
        XMM0, XMM1, XMM2,  XMM3,  XMM4,  XMM5,  XMM6,  XMM7,
        XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15,
    };

#endif

    static inline xmm argxmm(unsigned int argno) {
        FTL_ERROR_ON(argno >= param_xmms.size(), "argno out of bounds");
        return param_xmms[argno];
    }

    struct rm {
        const bool is_mem;
        const bool is_xmm;

        const int  r;
        const i64  offset;

        bool is_reg() const { return !is_mem && !is_xmm; }
        bool is_addressable() const { return fits_i32(offset); }

        rm(reg _r): is_mem(false), is_xmm(false), r(_r), offset(0) {}
        rm(xmm _r): is_mem(false), is_xmm(true),  r((reg)_r), offset(0) {}

        rm(reg base, i64 off): is_mem(true), is_xmm(false), r(base),
                offset(off) {
        }

        bool operator == (const rm& other) const;
        bool operator != (const rm& other) const;

    private:
        rm(); // disabled
    };

    inline bool rm::operator == (const rm& o) const {
        return is_mem == o.is_mem && r == o.r && offset == o.offset &&
               is_xmm == o.is_xmm;
    }

    inline bool rm::operator != (const rm& o) const {
        return !operator == (o);
    }

    static inline rm regop(reg r) {
        FTL_ERROR_ON(r >= NREGS, "invalid register id: %d", r);
        return rm(r);
    }

    static inline rm memop(reg base, i32 offset) {
        FTL_ERROR_ON(base >= NREGS, "invalid register id: %d", base);
        return rm(base, offset);
    }

}

std::ostream& operator << (std::ostream& os, const ftl::reg& r);
std::ostream& operator << (std::ostream& os, const ftl::xmm& r);
std::ostream& operator << (std::ostream& os, const ftl::rm& rm);

#endif
