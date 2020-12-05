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

#include "ftl/jitdump.h"

namespace ftl {

    static u64 timestamp_ns() {
        struct timespec tp;
        if (clock_gettime(CLOCK_MONOTONIC, &tp))
            FTL_ERROR("cannot read clock: %s (%d)", strerror(errno), errno);
        return (u64)tp.tv_sec * 1000000000ul + tp.tv_nsec;
    }

    jitdump::jitdump():
        m_mapdump(nullptr),
        m_jitdump(nullptr),
        m_mapper(nullptr),
        m_code_count(0) {

        std::string tmp = "/tmp/";
        std::string pid_str = std::to_string(getpid());

        std::string map_file = tmp + "perf-" + pid_str + ".map";
        std::string jit_file = tmp + "jit-" + pid_str + ".dump";

        m_mapdump = fopen(map_file.c_str(), "w+");
        m_jitdump = fopen(jit_file.c_str(), "w+");

        FTL_ERROR_ON(!m_mapdump, "Cannot open %s", map_file.c_str());
        FTL_ERROR_ON(!m_jitdump, "Cannot open %s", jit_file.c_str());

        int fd = fileno(m_jitdump);
        m_mapper = mmap(0, 4096, PROT_READ | PROT_EXEC, MAP_PRIVATE, fd, 0);
        if (m_mapper == MAP_FAILED)
            FTL_ERROR("mmap failed: %s (%d)", strerror(errno), errno);

        perf_jit_header header;
        memset(&header, 0, sizeof(header));

        header.magic = JIT_HEADER_MAGIC;
        header.version = JIT_HEADER_VERSION;
        header.size = sizeof(header);
        header.elf_mach = 0x3e; // x86_64
        header.pid = getpid();
        header.time_stamp = timestamp_ns();
        header.flags = 0;

        if (fwrite(&header, sizeof(header), 1, m_jitdump) != 1)
            FTL_ERROR("cannot write JIT header: %s", strerror(errno));
    }

    jitdump::~jitdump() {
        if (m_mapper != MAP_FAILED)
            munmap(m_mapper, 4096);
        if (m_jitdump)
            fclose(m_jitdump);
        if (m_mapdump)
            fclose(m_mapdump);
    }

    u64 jitdump::load(const string& name, void* code, size_t code_size) {
        if (!m_jitdump || !m_mapdump)
            return -1;

        fprintf(m_mapdump, "%lx %zx %s\n", (u64)code, code_size, name.c_str());

        perf_jit_load load;
        memset(&load, 0, sizeof(load));

        load.common.event = JIT_EVENT_LOAD;
        load.common.size = sizeof(load) + name.length() + 1 + code_size;
        load.common.time_stamp = timestamp_ns();
        load.pid = getpid();
        load.tid = pthread_self();
        load.vma = (uintptr_t)code;
        load.code_addr = (uintptr_t)code;
        load.code_size = code_size;
        load.code_idx = m_code_count++;

        if (fwrite(&load, sizeof(load), 1, m_jitdump) != 1)
            FTL_ERROR("cannot write data: %s (%d)", strerror(errno), errno);
        if (fwrite(name.c_str(), name.length() + 1, 1, m_jitdump) != 1)
            FTL_ERROR("cannot write name: %s (%d)", strerror(errno), errno);
        if (fwrite(code, code_size, 1, m_jitdump) != 1)
            FTL_ERROR("cannot write code: %s (%d)", strerror(errno), errno);

        return load.code_idx;
    }

    u64 jitdump::move(u64 id, void* prev, void* next, size_t size) {
        if (!m_jitdump || !m_mapdump)
            return -1;

        perf_jit_move move;
        memset(&move, 0, sizeof(move));

        move.common.event = JIT_EVENT_MOVE;
        move.common.size = sizeof(move);
        move.common.time_stamp = timestamp_ns();
        move.pid = getpid();
        move.tid = pthread_self();
        move.vma = (uintptr_t)next;
        move.old_code_addr = (uintptr_t)prev;
        move.new_code_addr = (uintptr_t)next;
        move.code_size = size;
        move.code_idx = id;

        if (fwrite(&move, sizeof(move), 1, m_jitdump) != 1)
            FTL_ERROR("cannot write data: %s (%d)", strerror(errno), errno);

        return id;
    }

}
