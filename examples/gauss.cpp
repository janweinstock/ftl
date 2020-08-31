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

#include <iostream>
#include <ftl.h>
#include <math.h>

using namespace ftl;

// helper to move immediate value into XMM register
void helper_movs(emitter& e, reg temp, xmm dest, double val) {
    e.movi(64, temp, f64_raw(val));
    e.movx(64, dest, temp);
}


int main() {
    cbuf code(1 * KiB);
    emitter emitter(code);
    fixup loop;
    u8* entry;

    typedef double (func)(double a, double b, double t, double p, int n);
    func* calc = (func*)code.get_code_ptr();

    helper_movs(emitter, RDX, XMM4, 2.0);
    helper_movs(emitter, RDX, XMM5, 4.0);

    entry = code.get_code_ptr();

    emitter.movs(64, XMM6, XMM0); // a2 = (a + b) / 2.0
    emitter.adds(64, XMM6, XMM1);
    emitter.divs(64, XMM6, XMM4);

    emitter.movs(64, XMM7, XMM0); // b2 = sqrt(a * b);
    emitter.muls(64, XMM7, XMM1);
    emitter.sqrt(64, XMM7, XMM7);

    emitter.movs(64, XMM8, XMM0); // t2 = p * (a - a2)^2
    emitter.subs(64, XMM8, XMM6);
    emitter.muls(64, XMM8, XMM8);
    emitter.muls(64, XMM8, XMM3);

    emitter.movs(64, XMM0, XMM6); // a = a2
    emitter.movs(64, XMM1, XMM7); // b = b2
    emitter.subs(64, XMM2, XMM8); // t = t - t2
    emitter.muls(64, XMM3, XMM4); // p = 2.0 * p

    emitter.decr(32, RDI);
    emitter.tstr(32, RDI, RDI); // repeat if n > 0
    emitter.jnz(-1, &loop);
    patch_jump(loop, entry);

    emitter.adds(64, XMM0, XMM1); // return (a + b)^2 / (4.0 * t)
    emitter.muls(64, XMM0, XMM0);
    emitter.divs(64, XMM0, XMM5);
    emitter.divs(64, XMM0, XMM2);

    emitter.ret();

    for (int i = 1; i < 4; i++) {
        double pi = calc(1.0, 1 / sqrt(2), 0.25, 1.0, i);
        printf("%d iterations: pi = %1.9f\n", i, pi);
    }

    return 0;
}

