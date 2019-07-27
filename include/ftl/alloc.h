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
#include "ftl/emitter.h"

namespace ftl {

    class alloc
    {
    private:
        emitter& m_emitter;
        value*   m_regmap[NREGS];
        u64      m_reguse[NREGS];
        u64      m_usecnt;
        u64      m_locals;
        u64      m_base;

        vector<value*> m_values;

        void validate(const value& val) const;

        // disabled
        alloc();
        alloc(const alloc&);

    public:
        const reg STACK_POINTER = RSP; // base address register for locals
        const reg BASE_REGISTER = RBX; // base address register for globals

        alloc(emitter& e);
        virtual ~alloc();

        u64  get_base_addr() const { return m_base; }
        void set_base_addr(u64 addr);

        bool is_local(const value& v) const;
        bool is_global(const value& v) const;

        void register_value(value* v);
        void unregister_value(value* v);

        value new_local(const string& name, int bits, i64 val, reg r = NREGS);
        value new_local_noinit(const string& name, int bits, reg r = NREGS);
        value new_global(const string& name, int bits, u64 addr);
        void free_value(value& val);

        bool is_empty(reg r) const;
        bool is_dirty(reg r) const;

        size_t count_dirty_regs() const;
        size_t count_active_regs() const;

        reg select();
        void assign(reg r, value* val);
        void free(reg r);
        void use(reg r);
        void store(reg r);
        void flush(reg r);

        void store_volatile_regs();
        void flush_volatile_regs();

        void fetch(value& val, reg r = NREGS);
        void store(value& val);
        void flush(value& val);

        void prologue();
        void epilogue();

        void reset();
    };

    inline void alloc::set_base_addr(u64 addr) {
        FTL_ERROR_ON(m_base, "base address already set");
        m_base = addr;
    }

    inline bool alloc::is_local(const value& v) const {
        return v.m.r == STACK_POINTER;
    }

    inline bool alloc::is_global(const value& v) const {
        return v.m.r == BASE_REGISTER;
    }

    inline bool alloc::is_empty(reg r) const {
        if (m_regmap[r] == NULL)
            return true;
        if (m_regmap[r]->is_dead())
            return true;
        return false;
    }

    inline bool alloc::is_dirty(reg r) const {
        if (is_empty(r))
            return false;
        return m_regmap[r]->is_dirty();
    }

    inline size_t alloc::count_dirty_regs() const {
        size_t count = 0;
        for (auto reg : m_regmap)
            if (reg != NULL && reg->is_dirty())
                count++;
        return count;
    }

    inline size_t alloc::count_active_regs() const {
        size_t count = 0;
        for (auto reg : m_regmap)
            if (reg != NULL)
                count++;
        return count;
    }

}

#endif
