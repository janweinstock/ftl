 ##############################################################################
 #                                                                            #
 # Copyright 2020 Jan Henrik Weinstock                                        #
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

set(FTL_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(FTL_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(FTL_VERSION_PATCH ${PROJECT_VERSION_PATCH})

if (${FTL_VERSION_MINOR} LESS 10)
    set(FTL_VERSION_MINOR 0${FTL_VERSION_MINOR})
endif()

if (${FTL_VERSION_PATCH} LESS 10)
    set(FTL_VERSION_PATCH 0${FTL_VERSION_PATCH})
endif()

execute_process(COMMAND git rev-parse HEAD
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE FTL_GIT_REV
                ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND git rev-parse --short HEAD
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE FTL_GIT_REV_SHORT
                ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND git diff --quiet
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE FTL_GIT_DIRTY
                ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

if (FTL_GIT_DIRTY)
    string(APPEND FTL_GIT_REV "-dirty")
    string(APPEND FTL_GIT_REV_SHORT "-dirty")
endif()

string(CONCAT FTL_VERSION ${FTL_VERSION_MAJOR}
                          ${FTL_VERSION_MINOR}
                          ${FTL_VERSION_PATCH})

string(CONCAT FTL_VERSION_STRING ${FTL_VERSION_MAJOR} "."
                                 ${FTL_VERSION_MINOR} "."
                                 ${FTL_VERSION_PATCH} "-"
                                 ${FTL_GIT_REV_SHORT})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/ftl/version.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/gen/ftl/version.h @ONLY)

# message(STATUS "PROJECT_VERSION_MAJOR = " ${PROJECT_VERSION_MAJOR})
# message(STATUS "PROJECT_VERSION_MINOR = " ${PROJECT_VERSION_MINOR})
# message(STATUS "PROJECT_VERSION_PATCH = " ${PROJECT_VERSION_PATCH})
# message(STATUS "FTL_VERSION_MAJOR = " ${PROJECT_VERSION_MAJOR})
# message(STATUS "FTL_VERSION_MINOR = " ${PROJECT_VERSION_MINOR})
# message(STATUS "FTL_VERSION_PATCH = " ${PROJECT_VERSION_PATCH})
# message(STATUS "FTL_VERSION = " ${FTL_VERSION})
# message(STATUS "FTL_GIT_REV = " ${FTL_GIT_REV})
# message(STATUS "FTL_GIT_REV_SHORT = " ${FTL_GIT_REV_SHORT})
# message(STATUS "FTL_VERSION_STRING = " ${FTL_VERSION_STRING})
