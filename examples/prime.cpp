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

#define N 100

int number;

static func gen_isprime(cgen& code) {
    label loop = code.gen_label("loop");
    label is_prime = code.gen_label("is_prime");
    label no_prime = code.gen_label("no_prime");

    func isprime = code.gen_function("isprime");

    value n = code.gen_global_i32(&number);
    value i = code.gen_local_i32(2);

    loop.place();
    code.gen_cmp(i, n);
    code.gen_jge(is_prime);

    value r = code.gen_local_i32(0);
    code.gen_add(r, n);
    code.gen_umod(r, i);
    code.gen_tst(r, r);
    code.gen_jz(no_prime);

    code.gen_add(i, 1);
    code.gen_jmp(loop);

    is_prime.place();
    code.gen_ret(1);

    no_prime.place();
    code.gen_ret(0);

    code.free_value(r);
    code.free_value(i);
    code.free_value(n);

    return isprime;
}

int main() {
    cgen code(4 * KiB);
    func isprime = gen_isprime(code);

    std::cout << "prime numbers <" << N << std::endl;
    for (number = 2; number < N; number++)
        if (isprime())
            std::cout << number << std::endl;

    return 0;
}
