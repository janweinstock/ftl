 ##############################################################################
 #                                                                            #
 # Copyright 2018 Jan Henrik Weinstock                                        #
 #                                                                            #
 # Licensed under the Apache License, Version 2.0 (the "License");            #
 # you may not use this file except in compliance with the License.           #
 # You may obtain a copy of the License at                                    #
 #                                                                            #
 #     http://www.apache.org/licenses/LICENSE-2.0                             #
 #                                                                            #
 # Unless required by applicable law or agreed to in writing, software        #
 # distributed under the License is distributed on an "AS IS" BASIS,          #
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   #
 # See the License for the specific language governing permissions and        #
 # limitations under the License.                                             #
 #                                                                            #
 ##############################################################################

macro(basic_test nm)
    add_executable(${nm} ${nm}.cpp)
    target_link_libraries(${nm} ftl gtest gtest_main)
    add_test(NAME basic/${nm} COMMAND $<TARGET_FILE:${nm}>)
    set_tests_properties(basic/${nm} PROPERTIES TIMEOUT 30)
endmacro()

basic_test(hello)
basic_test(bitops)
basic_test(emitter)
basic_test(immops)
basic_test(iopsmem)
basic_test(shift)
basic_test(jump)
basic_test(muldiv)
basic_test(cgen)
basic_test(call)
basic_test(loops)
basic_test(setcc)
basic_test(movext)
basic_test(cmov)
basic_test(fence)
basic_test(atomic)
basic_test(scratch)
basic_test(fp)
basic_test(scalar)

