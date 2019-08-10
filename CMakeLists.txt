 ##############################################################################
 #                                                                            #
 # Copyright 2019 Jan Henrik Weinstock                                        #
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

project(ftl)
cmake_minimum_required(VERSION 3.5)

option(BUILD_TESTS "Build unit tests, requires GTest and GMock >= 1.8.0" ON)
option(BUILD_EXAMPLES "Build example programs" ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

set(src "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(inc "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(sources
    "${src}/ftl/utils.cpp"
    "${src}/ftl/reg.cpp"
    "${src}/ftl/cbuf.cpp"
    "${src}/ftl/emitter.cpp"
    "${src}/ftl/label.cpp"
    "${src}/ftl/value.cpp"
    "${src}/ftl/alloc.cpp"
    "${src}/ftl/func.cpp")

add_library(ftl ${sources})

target_include_directories(ftl PUBLIC ${inc})

install(TARGETS ftl DESTINATION lib)
install(DIRECTORY ${inc}/ DESTINATION include)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
