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

#ifndef FTL_JITDUMP_H
#define FTL_JITDUMP_H

#include "ftl/common.h"
#include "ftl/error.h"
#include "ftl/func.h"

namespace ftl {

    class jitdump // singleton
    {
    private:
        mutex       m_mutex;
        atomic<u64> m_counter;
        FILE*       m_mapdump;
        FILE*       m_jitdump;
        void*       m_mapper;

        enum : u32 {
            JIT_HEADER_MAGIC = 0x4a695444,
            JIT_HEADER_VERSION = 1,
        };

        enum : u32 {
            JIT_EVENT_LOAD = 0,
            JIT_EVENT_MOVE = 1,
            JIT_EVENT_DEBUG_INFO = 2,
            JIT_EVENT_CLOSE = 3,
            JIT_EVENT_UNWINDING_INFO = 4,
        };

        struct perf_jit_header {
            u32 magic;
            u32 version;
            u32 size;
            u32 elf_mach;
            u32 pad0;
            u32 pid;
            u64 time_stamp;
            u64 flags;
        };

        struct perf_jit_common {
            u32 event;
            u32 size;
            u64 time_stamp;
        };

        struct perf_jit_load {
            perf_jit_common common;

            u32 pid;
            u32 tid;
            u64 vma;

            u64 code_addr;
            u64 code_size;
            u64 code_idx;
        };

        struct perf_jit_move {
            perf_jit_common common;

            u32 pid;
            u32 tid;
            u64 vma;

            u64 old_code_addr;
            u64 new_code_addr;

            u64 code_size;
            u64 code_idx;
        };

        jitdump();
        ~jitdump();

        jitdump(const jitdump&) = delete;
        jitdump& operator = (const jitdump&) = delete;

    public:
        u64 load(const string& name, void* code, size_t size);
        u64 move(u64 id, void* prev, void* next, size_t size);

        u64 load(const func& fn);

        static jitdump& instance();
    };

    inline u64 jitdump::load(const func& fn) {
        return load(fn.name(), fn.entry(), fn.size());
    }

}

#endif
