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

find_package(GTest "1.8.0" REQUIRED)
find_package(GMock "1.8.0" REQUIRED)
include_directories(SYSTEM ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})

set(tests test_hello test_bitops test_emitter test_immops test_iopsmem
    test_shift test_jump test_muldiv test_cgen test_call test_loops test_setcc
    test_movext)

foreach(test ${tests})
    add_executable(${test} ${test}.cpp)
    add_dependencies(${test} ftl)
    target_link_libraries(${test} ftl ${GMOCK_LIBRARIES} -pthread)
    add_test(NAME ${test} COMMAND ${test} ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(${test} PROPERTIES TIMEOUT 30)
endforeach()
