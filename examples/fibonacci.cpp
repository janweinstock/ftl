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

#include <iostream>
#include <ftl.h>

using namespace ftl;

int main() {
    cache code(1 * KiB);
    emitter emitter(code);
    fixup jump0, jump1;

    typedef int (func)(int);
    func* fib = (func*)code.get_code_ptr();

    // if (n < 1) return 0;
    emitter.movi(32, RAX, 0);
    emitter.tstr(32, RDI, RDI);
    emitter.jle(-1, &jump0);

    // if (n == 1) return 1;
    emitter.movi(32, RAX, 1);
    emitter.cmpr(32, RDI, RAX);
    emitter.je(-1, &jump1);

    // RAX = fib(n - 1);
    emitter.push(RDI);
    emitter.subi(32, RDI, 1);
    emitter.call((u8*)fib);
    emitter.pop(RDI);

    // RCX = fib(n - 2);
    emitter.push(RAX);
    emitter.push(RDI);
    emitter.subi(32, RDI, 2);
    emitter.call((u8*)fib);
    emitter.pop(RDI);
    emitter.pop(RCX);

    // return RAX + RCX
    emitter.addr(32, RAX, RCX);
    patch_jump(jump0, code.get_code_ptr());
    patch_jump(jump1, code.get_code_ptr());
    emitter.ret();

    for (int i = 1; i < 10; i++)
        std::cout << i << ": " << fib(i) << std::endl;

    return 0;
}
