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

using namespace ftl;

void output(void* base, int iteration, double val1, double val2) {
    printf("%d: sqrt(%f) = %f\n", iteration, val1, val2);
}

int main() {
    func simplefp("simplefp");
    auto value1 = simplefp.gen_local_f64("value1", 0.0);
    auto lcount = simplefp.gen_local_i32("lcount", 4);
    auto lentry = simplefp.gen_label("lentry");

    lentry.place();

    auto incval = simplefp.gen_scratch_f64("incval");
    auto value2 = simplefp.gen_scratch_f64("value2");

    simplefp.gen_cvt(incval, lcount);
    simplefp.gen_add(value1, incval);
    simplefp.gen_mul(value1, incval);
    simplefp.gen_sqrt(value2, value1);
    simplefp.gen_call(&output, lcount, value1, value2);
    simplefp.gen_dec(lcount);
    simplefp.gen_tst(lcount, lcount);
    simplefp.gen_ja(lentry);
    simplefp.gen_ret();

    simplefp();

    return 0;
}

