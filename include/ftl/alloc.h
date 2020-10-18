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
#include "ftl/ralloc.h"

namespace ftl {

    class alloc
    {
    private:
        emitter&    m_emitter;
        ralloc<reg> m_regs;
        ralloc<xmm> m_xmms;

        u64         m_locals;
        u64         m_base;

    public:
        alloc(emitter& e);
        alloc(alloc&&) = default;
        ~alloc();

        alloc() = delete;
        alloc(const alloc&) = delete;

        emitter& get_emitter() const { return m_emitter; }

        bool is_empty(reg r) const { return m_regs.is_empty(r); }
        bool is_empty(xmm r) const { return m_xmms.is_empty(r); }

        bool is_dirty(reg r) const { return m_regs.is_dirty(r); }
        bool is_dirty(xmm r) const { return m_xmms.is_dirty(r); }

        void mark_dirty(reg r) { m_regs.mark_dirty(r); }
        void mark_dirty(xmm r) { m_xmms.mark_dirty(r); }

        void mark_clean(reg r) { m_regs.mark_clean(r); }
        void mark_clean(xmm r) { m_xmms.mark_clean(r); }

        void block(reg r) { m_regs.block(r); }
        void block(xmm r) { m_xmms.block(r); }

        void unblock(reg r) { m_regs.unblock(r); }
        void unblock(xmm r) { m_xmms.unblock(r); }

        bool is_blocked(reg r) const { return m_regs.is_blocked(r); }
        bool is_blocked(xmm r) const { return m_xmms.is_blocked(r); }

        reg  select()     const { return m_regs.select(); }
        xmm  select_xmm() const { return m_xmms.select(); }

        reg  lookup(const value* val) { return m_regs.lookup(val); }
        xmm  lookup(const scalar* val) { return m_xmms.lookup(val); }

        reg  assign(const value* val, reg r = NREGS);
        xmm  assign(const scalar* val, xmm r = NXMM);

        reg  fetch(const value* val, reg r = NREGS);
        xmm  fetch(const scalar* val, xmm r = NXMM);

        void store(reg r);
        void store(xmm r);

        void flush(reg r);
        void flush(xmm r);

        void register_value(value* val) { m_regs.register_value(val); }
        void register_value(scalar* val) { m_xmms.register_value(val); }

        void unregister_value(value* val) { m_regs.unregister_value(val); }
        void unregister_value(scalar* val) { m_xmms.unregister_value(val); }


        u64  get_base_addr() const { return m_base; }
        void set_base_addr(u64 addr);

        value new_local_noinit(const string& name, int bits, reg r = NREGS);
        value new_local(const string& name, int bits, i64 val, reg r = NREGS);
        value new_global(const string& name, int bits, u64 addr);
        value new_scratch_noinit(const string& name, int bits, reg r = NREGS);
        value new_scratch(const string& nm, int bits, i64 val, reg r = NREGS);

        scalar new_local_scalar_noinit(const string& nm, int w, xmm r = NXMM);
        scalar new_local_scalar(const string& nm, int w, f64 f, xmm r = NXMM);
        scalar new_global_scalar(const string& nm, int w, u64 addr);
        scalar new_scratch_scalar_noinit(const string& n, int w, xmm r = NXMM);
        scalar new_scratch_scalar(const string& n, int w, f64 f, xmm r = NXMM);

        void free_value(value& val);
        void free_scalar(scalar& val);

        size_t count_active_regs() const;
        size_t count_dirty_regs() const;

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
