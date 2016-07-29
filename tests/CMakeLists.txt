#############################################################################
## Copyright (c) 2016, Fougue Ltd. <http://www.fougue.pro>
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
if(WIN32 AND GMIO_BUILD_SHARED_LIBS)
    configure_file(win_ctest_cmd.bat.cmake win_ctest_cmd.bat @ONLY)
    set(CMAKE_CTEST_COMMAND win_ctest_cmd.bat)
endif()
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

include_directories(${CMAKE_BINARY_DIR}/src/gmio_core) # For generated cmake headers

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/temp)

# test_core
set(GMIO_TEST_CORE_SRC
        main_test_core.c
        core_utils.c
        stream_buffer.c
        ../benchmarks/commons/benchmark_tools.c)
if(GMIO_BUILD_SHARED_LIBS)
    # Note_1: MSVC wants it because internal symbols are not exported (GCC
    #         exports all symbols)
    if(GMIO_STR2FLOAT_LIBCODE EQUAL 2 OR GMIO_FLOAT2STR_LIBCODE EQUAL 2)
        file(GLOB  GMIO_GOOGLE_DBLCONV_SRC  ../src/3rdparty/double-conversion/*.cc)
        set(GMIO_TEST_CORE_SRC
                ${GMIO_TEST_CORE_SRC}
                ${GMIO_GOOGLE_DBLCONV_SRC}
                ../src/gmio_core/internal/google_doubleconversion.cpp)
    endif()
    set(GMIO_TEST_CORE_SRC
            ${GMIO_TEST_CORE_SRC}
            ../src/3rdparty/miloyip_itoa/branchlut.c
            ../src/3rdparty/base64/b64.c
            ../src/gmio_core/internal/error_check.c
            ../src/gmio_core/internal/locale_utils.c
            ../src/gmio_core/internal/numeric_utils.c
            ../src/gmio_core/internal/ostringstream.c
            ../src/gmio_core/internal/stringstream.c)
endif()
add_executable(test_core EXCLUDE_FROM_ALL ${GMIO_TEST_CORE_SRC})
target_link_libraries(test_core gmio)

# test_stl
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/models
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
set(GMIO_TEST_STL_SRC
        main_test_stl.c
        stl_testcases.c
        core_utils.c
        stl_utils.c)
if(GMIO_BUILD_SHARED_LIBS)
    # See Note_1
    set(GMIO_TEST_STL_SRC
            ${GMIO_TEST_STL_SRC}
            ../src/gmio_core/internal/locale_utils.c
            ../src/gmio_core/internal/numeric_utils.c
            ../src/gmio_stl/internal/stl_error_check.c)
endif()
add_executable(test_stl EXCLUDE_FROM_ALL ${GMIO_TEST_STL_SRC})
target_link_libraries(test_stl gmio)

# test_amf
set(GMIO_TEST_AMF_SRC
        main_test_amf.c
        stream_buffer.c)
add_executable(test_amf EXCLUDE_FROM_ALL ${GMIO_TEST_AMF_SRC})
target_link_libraries(test_amf gmio)

# fake_support
if(GMIO_BUILD_TESTS_FAKE_SUPPORT)
    add_subdirectory(fake_support)
    add_dependencies(check fake_support)
endif()

# For some targets, GCC requires -lm
if(CMAKE_C_COMPILER_IS_GCC_COMPATIBLE)
    target_link_libraries(test_core m)
    target_link_libraries(test_stl m)
endif()

# Declare tests
add_test(NAME test_core  COMMAND test_core)
add_test(NAME test_stl   COMMAND test_stl)
add_test(NAME test_amf   COMMAND test_amf)
add_dependencies(check test_core test_stl test_amf)
