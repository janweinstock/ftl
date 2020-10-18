/******************************************************************************
 *                                                                            *
 * Copyright 2020 Jan Henrik Weinstock                                        *
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

#ifndef FTL_RALLOC_H
#define FTL_RALLOC_H

#include "ftl/common.h"
#include "ftl/bitops.h"
#include "ftl/error.h"
#include "ftl/utils.h"

#include "ftl/reg.h"
#include "ftl/value.h"
#include "ftl/scalar.h"
#include "ftl/emitter.h"

namespace ftl {

    template <typename REG>
    struct reg_traits;

    template <>
    struct reg_traits<reg> {
        typedef reg   reg_type;
        typedef value val_type;
        static const reg NREGS = ftl::NREGS;
    };

    template <>
    struct reg_traits<xmm> {
        typedef xmm    reg_type;
        typedef scalar val_type;
        static const xmm NREGS = ftl::NXMM;
    };

    template <typename REG>
    class ralloc {
    public:
        typedef typename reg_traits<REG>::reg_type reg_type;
        typedef typename reg_traits<REG>::val_type val_type;

        static const REG NREGS = reg_traits<REG>::NREGS;

        bool is_valid(REG r) const;
        bool is_empty(REG r) const;
        bool is_dirty(REG r) const;

        void mark_dirty(REG r);
        void mark_clean(REG r);

        REG  select() const;
        REG  lookup(const val_type* val) const;
        REG  assign(REG r, const val_type* val);
        REG  fetch(const val_type* val, REG r = NREGS);

        const val_type* lookup(REG r) const;

        void reset();

        ralloc(emitter& e);
        ralloc(ralloc<REG>&&) = default;
        ralloc() = delete;
        ralloc(const ralloc&) = delete;

        void register_value(val_type* v);
        void unregister_value(val_type* v);

        void block(REG r)            { m_blocked.insert(r); }
        void unblock(REG r)          { m_blocked.erase(r); }
        bool is_blocked(REG r) const { return m_blocked.count(r) > 0; }

        size_t count_active_regs() const;
        size_t count_dirty_regs() const;

    private:
        struct reginfo {
            REG             regid;
            const val_type* owner;
            bool            dirty;
            mutable u64     count;
        };

        reginfo         m_regmap[NREGS];
        mutable u64     m_usecnt;
        emitter&        m_emitter;
        set<val_type*>  m_values;
        set<REG>        m_blocked;

        REG select(const vector<REG>& order) const;
    };

    template <typename REG>
    inline bool ralloc<REG>::is_valid(REG r) const {
        return r < NREGS;
    }

    template <typename REG>
    inline bool ralloc<REG>::is_empty(REG r) const {
        FTL_ERROR_ON(!is_valid(r), "invalid register specified");
        if (m_regmap[r].owner == nullptr)
            return true;
        if (m_regmap[r].owner->is_dead())
            return true;
        return false;
    }

    template <typename REG>
    inline bool ralloc<REG>::is_dirty(REG r) const {
        FTL_ERROR_ON(!is_valid(r), "invalid register specified");
        if (is_empty(r))
            return false;
        return m_regmap[r].dirty;
    }

    template <typename REG>
    inline void ralloc<REG>::mark_dirty(REG r) {
        FTL_ERROR_ON(!is_valid(r), "invalid register specified");
        m_regmap[r].dirty = true;
    }

    template <typename REG>
    inline void ralloc<REG>::mark_clean(REG r) {
        FTL_ERROR_ON(!is_valid(r), "invalid register specified");
        m_regmap[r].dirty = false;
    }

    template <>
    inline reg ralloc<reg>::select() const {
        static const vector<reg> alloc_order = {
            RBX, RCX, RDX, RAX, RDI, RSI,  R8,  R9,
            R10, R11, R14, R15, R12, R13, RSP, RBP,
        };

        return select(alloc_order);
    }

    template <>
    inline xmm ralloc<xmm>::select() const {
        static const vector<xmm> alloc_order = {
            XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15,
            XMM7, XMM6, XMM5,  XMM4,  XMM3,  XMM2,  XMM1,  XMM0,
        };

        return select(alloc_order);
    }

    template <typename REG>
    inline REG ralloc<REG>::lookup(const val_type* v) const {
        if (v == nullptr)
            return NREGS;

        for (auto info : m_regmap) {
            if (info.owner == v) {
                info.count = m_usecnt++;
                return info.regid;
            }
        }

        return NREGS;
    }

    template <typename REG>
    inline REG ralloc<REG>::assign(REG r, const val_type* val) {
        FTL_ERROR_ON(!is_valid(r), "invalid register specified");

        m_regmap[r].owner = val;
        m_regmap[r].dirty = false;
        m_regmap[r].count = val ? m_usecnt++ : 0;

        return r;
    }

    template <typename REG>
    inline REG ralloc<REG>::fetch(const val_type* val, REG r) {
        FTL_ERROR_ON(!val, "attempt to fetch nullptr value");

        REG curr = lookup(val);
        if ((curr < NREGS) && (curr == r || r == NREGS))
            return curr;

        r = assign(val, r);

        if (curr < NREGS && curr != r) {
            m_emitter.movr(val->bits, r, curr);
        } else {
            FTL_ERROR_ON(val->is_scratch(), "attempt to fetch scratch scalar");
            m_emitter.movr(val->bits, r, val->mem());
        }

        return r;
    }

    template <typename REG> inline
    const typename ralloc<REG>::val_type* ralloc<REG>::lookup(REG r) const {
        FTL_ERROR_ON(!is_valid(r), "invalid register specified");
        return m_regmap[r].owner;
    }

    template <typename REG>
    inline ralloc<REG>::ralloc(emitter& e):
        m_regmap(),
        m_usecnt(0),
        m_emitter(e) {
        reset();
    }

    template <>
    inline ralloc<reg>::ralloc(emitter& e):
        m_regmap(),
        m_usecnt(0),
        m_emitter(e) {
        reset();
        block(BASE_POINTER);
        block(STACK_POINTER);
    }


    template <typename REG>
    inline void ralloc<REG>::reset() {
        for (auto val : m_values)
            val->mark_dead();

        m_values.clear();
        m_usecnt = 0;

        for (int r = 0; r < NREGS; r++) {
           m_regmap[r].regid = (REG)r;
           m_regmap[r].owner = nullptr;
           m_regmap[r].dirty = false;
           m_regmap[r].count = 0;
       }
    }

    template <typename REG>
    inline void ralloc<REG>::register_value(val_type* v) {
        if (stl_contains(m_values, v))
            FTL_ERROR("attempt to register value '%s' twice", v->name());
        m_values.insert(v);
    }

    template <typename REG>
    inline void ralloc<REG>::unregister_value(val_type* v) {
        if (!stl_contains(m_values, v))
            FTL_ERROR("attempt to unregister unknown value '%s'", v->name());
        m_values.erase(v);
    }

    template <typename REG>
    inline size_t ralloc<REG>::count_active_regs() const {
        size_t count = 0;
        for (int i = 0; i < NREGS; i++)
            if (m_regmap[i].owner != nullptr)
                count++;
        return count;
    }

    template <typename REG>
    inline size_t ralloc<REG>::count_dirty_regs() const {
        size_t count = 0;
        for (int i = 0; i < NREGS; i++)
            if (m_regmap[i].owner != nullptr && m_regmap[i].dirty)
                count++;
        return count;
    }

    template <typename REG>
    inline REG ralloc<REG>::select(const vector<REG>& order) const {
        vector<REG> regs;
        std::copy_if(order.begin(), order.end(), std::back_inserter(regs),
                     [this](REG r) -> bool {
            return !is_blocked(r);
        });

        // try unused registers first
        for (REG r : regs)
            if (is_empty(r))
                return r;

        // next, try registers that do not need to be flushed
        for (REG r : regs)
            if (!is_dirty(r))
                return r;

        // pick least recently used
        REG lru = NREGS;
        u64 min = ~0ull;
        for (REG r : regs) {
            if (m_regmap[r].count < min) {
                min = m_regmap[r].count;
                lru = r;
            }
        }

        FTL_ERROR_ON(!is_valid(lru), "failed to select a register");
        return lru;
    }

}

#endif
