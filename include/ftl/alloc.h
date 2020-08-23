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

#ifndef FTL_ALLOC_H
#define FTL_ALLOC_H

#include "ftl/common.h"
#include "ftl/bitops.h"
#include "ftl/error.h"
#include "ftl/utils.h"

#include "ftl/reg.h"
#include "ftl/value.h"
#include "ftl/scalar.h"
#include "ftl/emitter.h"

namespace ftl {

    class alloc
    {
    private:
        template <typename REG, typename VAL>
        struct reginfo {
            REG         regid;
            const VAL*  owner;
            bool        dirty;
            mutable u64 count;
        };

        emitter&             m_emitter;
        reginfo<reg, value>  m_regmap[NREGS];
        reginfo<xmm, scalar> m_xmmmap[NXMM];
        mutable u64          m_usecnt;

        u64 m_locals;
        u64 m_base;

        set<value*>  m_values;
        set<scalar*> m_scalars;

        set<reg> m_blocked_regs;
        set<xmm> m_blocked_xmms;

    public:
        const reg STACK_POINTER = RSP; // base address register for locals
        const reg BASE_REGISTER = RBX; // base address register for globals

        alloc(emitter& e);
        alloc(alloc&&) = default;
        ~alloc();

        alloc() = delete;
        alloc(const alloc&) = delete;

        void block(reg r) { m_blocked_regs.insert(r); }
        void block(xmm r) { m_blocked_xmms.insert(r); }

        void unblock(reg r) { m_blocked_regs.erase(r); }
        void unblock(xmm r) { m_blocked_xmms.erase(r); }

        bool is_blocked(reg r) const { return m_blocked_regs.count(r) > 0; }
        bool is_blocked(xmm r) const { return m_blocked_xmms.count(r) > 0; }

        const set<reg>& get_blocked_regs() const { return m_blocked_regs; }
        const set<xmm>& get_blocked_xmms() const { return m_blocked_xmms; }

        set<reg> get_blocked_regs() { return m_blocked_regs; }
        set<xmm> get_blocked_xmms() { return m_blocked_xmms; }

        emitter& get_emitter() const { return m_emitter; }

        bool is_empty(reg r) const;
        bool is_dirty(reg r) const;
        bool is_empty(xmm r) const;
        bool is_dirty(xmm r) const;

        void mark_dirty(reg r);
        void mark_clean(reg r);
        void mark_dirty(xmm r);
        void mark_clean(xmm r);

        reg  select() const;
        xmm  select_xmm() const;

        reg  lookup(const value* val) const;
        reg  assign(const value* val, reg r = NREGS);
        reg  fetch(const value* val, reg r = NREGS);

        void free(reg r);
        void store(reg r);
        void flush(reg r);

        xmm  lookup(const scalar* val) const;
        xmm  assign(const scalar* val, xmm r = NXMM);
        xmm  fetch(const scalar* val, xmm r = NXMM);

        void free(xmm r);
        void store(xmm r);
        void flush(xmm r);

        u64  get_base_addr() const { return m_base; }
        void set_base_addr(u64 addr);

        void register_value(value* v);
        void unregister_value(value* v);
        void register_scalar(scalar* s);
        void unregister_scalar(scalar* s);

        value new_local_noinit(const string& name, int bits, reg r = NREGS);
        value new_local(const string& name, int bits, i64 val, reg r = NREGS);
        value new_global(const string& name, int bits, u64 addr);
        value new_scratch_noinit(const string& name, int bits, reg r = NREGS);
        value new_scratch(const string& name, int bits, i64 val, reg r = NREGS);

        void free_value(value& val);
        void free_scalar(scalar& val);

        size_t count_dirty_regs() const;
        size_t count_active_regs() const;

        void store_all_regs();
        void flush_all_regs();

        void store_volatile_regs();
        void flush_volatile_regs();

        void reset();
    };

    inline void alloc::set_base_addr(u64 addr) {
        FTL_ERROR_ON(m_base, "base address already set");
        m_base = addr;
    }

}

#endif
