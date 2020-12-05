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

cmake_minimum_required(VERSION 3.8)
project(ftl VERSION 2020.05.01 LANGUAGES CXX)

option(FTL_BUILD_TESTS "Build unit tests" OFF)
option(FTL_BUILD_EXAMPLES "Build example programs" OFF)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(GenVersionHeader)

set(inc "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(gen "${CMAKE_CURRENT_BINARY_DIR}/gen")

set(sources
    "src/ftl/utils.cpp"
    "src/ftl/reg.cpp"
    "src/ftl/cbuf.cpp"
    "src/ftl/emitter.cpp"
    "src/ftl/label.cpp"
    "src/ftl/value.cpp"
    "src/ftl/scalar.cpp"
    "src/ftl/alloc.cpp"
    "src/ftl/func.cpp"
    "src/ftl/jitdump.cpp"
    "src/ftl/version.cpp")

add_library(ftl ${sources})
target_compile_features(ftl PUBLIC cxx_std_11)
target_compile_definitions(ftl PUBLIC $<$<CONFIG:DEBUG>:FTL_DEBUG>)
target_include_directories(ftl PUBLIC ${inc})
target_include_directories(ftl PUBLIC ${gen})
set_target_properties(ftl PROPERTIES DEBUG_POSTFIX "d")

install(TARGETS ftl DESTINATION lib)
install(DIRECTORY ${inc}/ DESTINATION include)
install(DIRECTORY ${gen}/ DESTINATION include)

if(FTL_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

if(FTL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
