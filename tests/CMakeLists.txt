#############################################################################
## Copyright (c) 2017, Fougue Ltd. <http://www.fougue.pro>
## All rights reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
##
##     1. Redistributions of source code must retain the above copyright
##        notice, this list of conditions and the following disclaimer.
##
##     2. Redistributions in binary form must reproduce the above
##        copyright notice, this list of conditions and the following
##        disclaimer in the documentation and/or other materials provided
##        with the distribution.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
## "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
## A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
## OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
## SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
## LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
## DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
## THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#############################################################################

enable_testing()
set(CMAKE_CTEST_COMMAND ctest -V --timeout 120)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

include_directories(${CMAKE_BINARY_DIR}/src/gmio_core) # For generated cmake headers
include_directories(${ZLIB_INCLUDE_DIRS})

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/temp)

# test_core
set(GMIO_TEST_CORE_SRC
        main_test_core.cpp
        ../benchmarks/commons/benchmark_tools.cpp)
add_executable(test_core EXCLUDE_FROM_ALL ${GMIO_TEST_CORE_SRC})
target_link_libraries(test_core gmio_static)

# test_stl
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/models
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
set(GMIO_TEST_STL_SRC
        main_test_stl.cpp
        stl_testcases.cpp
        stl_utils.cpp)
add_executable(test_stl EXCLUDE_FROM_ALL ${GMIO_TEST_STL_SRC})
target_link_libraries(test_stl gmio_static)

# test_amf
#set(GMIO_TEST_AMF_SRC
#        main_test_amf.c
#        stream_buffer.c)
#add_executable(test_amf EXCLUDE_FROM_ALL ${GMIO_TEST_AMF_SRC})
#target_link_libraries(test_amf gmio_static)
#target_link_libraries(test_amf ${ZLIB_LIBRARIES})

# fake_support
#if(GMIO_BUILD_TESTS_FAKE_SUPPORT)
#    add_subdirectory(fake_support)
#    add_dependencies(check fake_support)
#endif()

# For some targets, GCC requires -lm
if(CMAKE_CXX_COMPILER_IS_GCC_COMPATIBLE)
    target_link_libraries(test_core m)
    target_link_libraries(test_stl m)
endif()

# Declare tests
add_test(NAME test_core  COMMAND test_core)
add_test(NAME test_stl   COMMAND test_stl)
#add_test(NAME test_amf   COMMAND test_amf)
add_dependencies(check test_core test_stl)# test_amf)
