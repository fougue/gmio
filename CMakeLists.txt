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

cmake_minimum_required(VERSION 2.8)
#cmake_policy(SET CMP0018 NEW)

project(gmio)

include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckCXXSourceCompiles)
include(CheckTypeSize)
include(CMakeDependentOption)

set(GMIO_VERSION_MAJOR 0)
set(GMIO_VERSION_MINOR 5)
set(GMIO_VERSION_PATCH 0)
set(GMIO_VERSION
    ${GMIO_VERSION_MAJOR}.${GMIO_VERSION_MINOR}.${GMIO_VERSION_PATCH})

# C++ Compiler detection
if(CMAKE_CXX_COMPILER MATCHES ".*clang")
    set(CMAKE_COMPILER_IS_CLANG 1)
endif ()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
    set(CMAKE_CXX_COMPILER_IS_GCC_COMPATIBLE 1) # Compatible with GNUCXX options
endif()

# Build type
string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if(CMAKE_BUILD_TYPE_LOWER MATCHES "debug")
    set(GMIO_DEBUG_BUILD 1)
else()
    set(GMIO_DEBUG_BUILD 0)
endif()

# Options
option(GMIO_BUILD_DLL         "Build gmio also as a shared library(DLL)" ON)
option(GMIO_BUILD_BENCHMARKS  "Build performance benchmarks for the gmio library" OFF)
option(GMIO_BUILD_EXAMPLES    "Build gmio examples" OFF)
option(GMIO_BUILD_TESTS_FAKE_SUPPORT  "Build tests/fake_support target" OFF)
if(CMAKE_CXX_COMPILER_IS_GCC_COMPATIBLE)
    option(GMIO_BUILD_TESTS_COVERAGE "Instrument testing code with code coverage" OFF)
endif()
option(GMIO_USE_BUNDLED_ZLIB "Use bundled version of zlib in gmio" ON)

# Declare variable GMIO_STR2FLOAT_LIB(library for string-to-float conversion)
#     - std:
#           C standard library functions(eg strtod())
#     - irrlicht_fast_atof:
#           fast_atof() function of Irrlicht project
#     - google_doubleconversion:
#           Google's doubleconversion functions, note that this implies C++
#    fastness: std < google_doubleconversion < irrlicht_fast_atof
#    robustness: irrlicht_fast_atof < google_doubleconversion <= std
set(GMIO_STR2FLOAT_LIB "irrlicht_fast_atof" CACHE STRING "String->float library to use")
set_property(CACHE GMIO_STR2FLOAT_LIB
                   PROPERTY STRINGS std irrlicht_fast_atof google_doubleconversion)

if(GMIO_STR2FLOAT_LIB MATCHES "std")
    set(GMIO_STR2FLOAT_LIBCODE 0)
elseif(GMIO_STR2FLOAT_LIB MATCHES "irrlicht_fast_atof")
    set(GMIO_STR2FLOAT_LIBCODE 1)
elseif(GMIO_STR2FLOAT_LIB MATCHES "google_doubleconversion")
    set(GMIO_STR2FLOAT_LIBCODE 2)
endif()

# Declare variable GMIO_FLOAT2STR_LIB(library for float-to-string conversion)
#     - std:
#           C standard library functions(eg snprintf())
#     - google_doubleconversion:
#           Google's doubleconversion functions, note that this implies C++
#    fastness: std < google_doubleconversion
#    robustness: google_doubleconversion <= std
set(GMIO_FLOAT2STR_LIB "std" CACHE STRING "Float->string library to use")
set_property(CACHE GMIO_FLOAT2STR_LIB
                   PROPERTY STRINGS std google_doubleconversion)

if(GMIO_FLOAT2STR_LIB MATCHES "std")
    set(GMIO_FLOAT2STR_LIBCODE 0)
elseif(GMIO_FLOAT2STR_LIB MATCHES "google_doubleconversion")
    set(GMIO_FLOAT2STR_LIBCODE 2)
endif()

# Find bit size of the target architecture
math(EXPR GMIO_TARGET_ARCH_BIT_SIZE "8 * ${CMAKE_SIZEOF_VOID_P}")
message(STATUS "GMIO_TARGET_ARCH_BIT_SIZE = ${GMIO_TARGET_ARCH_BIT_SIZE}")

# Test if host's architecture is big-endian
include(TestBigEndian)
test_big_endian(GMIO_HOST_IS_BIG_ENDIAN)

# Adapt flags for GCC
if(CMAKE_CXX_COMPILER_IS_GCC_COMPATIBLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors")
    if(CMAKE_COMPILER_IS_GNUCC)
        add_definitions(-D_POSIX_SOURCE)
    endif()

    # cmake < v2.8.9 does not support CMAKE_POSITION_INDEPENDENT_CODE
    #     For this project, gcc needs -fPIC option on x86_64 UNIX platform
    if(NOT (WIN32 OR CYGWIN)
           AND (CMAKE_VERSION VERSION_LESS 2.8.9)
           AND ((${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86")
                OR (${CMAKE_SYSTEM_PROCESSOR} MATCHES "i.86"))
           AND (GMIO_TARGET_ARCH_BIT_SIZE EQUAL 64))
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_SHARED_LIBRARY_C_FLAGS}")
    endif()

    # Force PIC
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Adapt flags for GCC-like compilers
if(CMAKE_CXX_COMPILER_IS_GCC_COMPATIBLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstrict-aliasing")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wstrict-aliasing")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align -Wfloat-equal")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winline")
    if(CMAKE_COMPILER_IS_GNUCXX)
        # Undefined behavior sanitizer(ubsan), requires GCC >= v4.9
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlogical-op")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlogical-op-parentheses")
    endif()
endif()

# Adapt flags for Clang
if(CMAKE_COMPILER_IS_CLANG)
    # Clang on apple-darwin13.4.0 wrongly reports many unused functions
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")
endif()

# Adapt flags for Visual C++
if(MSVC)
    # Set warning level to /W4
    string(REGEX REPLACE "/W[0-9]" "/W4" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

    # Set warning level to /Wall
    #string(REGEX REPLACE "/W[0-9]" "/Wall" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4255 /wd4710 /wd4711 /wd4820")

    # Enable static analysis
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /analyze")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Oi /Qvec-report:2")

    # Treat some warnings as errors
    # 'function' undefined; assuming extern returning int
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /we4013")
    # 'function' : different types for formal and actual parameter 'number'
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /we4024")
    # 'operator' : 'identifier1' indirection to slightly different base types from 'identifier2'
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /we4057")
    # 'type' : incompatible types - from 'type1' to 'type2'
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /we4133")

    # Enable /Zc:strictStrings when msvc_ver > 2012 and build_type != Debug
    if((MSVC_VERSION GREATER 1700) AND NOT (CMAKE_BUILD_TYPE STREQUAL "Debug"))
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:strictStrings")
    endif()

    # Disable deprecation warnings about "non-secure" CRT functions
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS)

    # Enable "Generate Intrinsic Functions"
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Oi")
endif()

# Check available POSIX features
if(UNIX)
    # See:
    #   http://linux.die.net/man/2/fstat64
    #   https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man2/fstat.2.html
    #   https://www.gnu.org/software/libc/manual/html_mono/libc.html#Feature-Test-Macros
    if(APPLE)
        add_definitions(-D_DARWIN_USE_64_BIT_INODE)
        list(APPEND CMAKE_REQUIRED_DEFINITIONS -D_DARWIN_USE_64_BIT_INODE)
    else()
        add_definitions(-D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1)
        list(APPEND CMAKE_REQUIRED_DEFINITIONS
                    -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1)
    endif()
endif()

set(GMIO_HAVE_SYS_TYPES_H ${HAVE_SYS_TYPES_H})
if(NOT DEFINED HAVE_SYS_TYPES_H)
    check_include_files(sys/types.h GMIO_HAVE_SYS_TYPES_H)
endif()

set(GMIO_HAVE_SYS_STAT_H ${HAVE_SYS_STAT_H})
if (NOT DEFINED HAVE_SYS_STAT_H)
    check_include_files(sys/stat.h GMIO_HAVE_SYS_STAT_H)
endif()

check_function_exists(fileno GMIO_HAVE_POSIX_FILENO)

# Have fstat64() ?
check_cxx_source_compiles(
    "#include <sys/stat.h>
     int main() { fstat64(0, NULL); return 0; }"
    GMIO_HAVE_POSIX_FSTAT64)
if(WIN32)
    check_function_exists(_fstat64 GMIO_HAVE_WIN__FSTAT64)
endif()

# Check size(in bytes) of stat::st_size
set(CMAKE_EXTRA_INCLUDE_FILES sys/stat.h)
if(GMIO_HAVE_WIN__FSTAT64)
    check_type_size("((struct _stat64*)0)->st_size" GMIO_SIZEOF_STRUCT_STAT_ST_SIZE)
elseif(GMIO_HAVE_POSIX_FSTAT64)
    check_type_size("((struct stat64*)0)->st_size" GMIO_SIZEOF_STRUCT_STAT_ST_SIZE)
else()
    check_type_size("((struct stat*)0)->st_size" GMIO_SIZEOF_STRUCT_STAT_ST_SIZE)
endif()
set(CMAKE_EXTRA_INCLUDE_FILES)
set(CMAKE_REQUIRED_DEFINITIONS)

# Verify large file support ?
if(GMIO_HAVE_SYS_STAT_H
       AND NOT(GMIO_SIZEOF_STRUCT_STAT_ST_SIZE STREQUAL "0") # Not arch-dependent size
       AND NOT(GMIO_SIZEOF_STRUCT_STAT_ST_SIZE EQUAL 8))
    message(WARNING "<sys/stat.h> does not provide 64b variant of fstat(), you may encounter problems with files > 2GB")
endif()

# Have compiler-intrisics byte swap functions ?
if(CMAKE_CXX_COMPILER_IS_GCC_COMPATIBLE)
    # __builtin_bswap16() is missing in x86 GCC version prior to v4.7
    # See http://gcc.gnu.org/bugzilla/show_bug.cgi?id=52624
    check_cxx_source_compiles(
        "int main() { return (int)__builtin_bswap16(0x1122); }"
        GMIO_HAVE_GCC_BUILTIN_BSWAP16)
    check_cxx_source_compiles(
        "int main() { return (int)__builtin_bswap32(0x11223344); }"
        GMIO_HAVE_GCC_BUILTIN_BSWAP32)
elseif(MSVC)
    check_cxx_source_compiles(
        "#include <cstdlib>
         int main() { return (int)_byteswap_ulong(0x11223344); }"
        GMIO_HAVE_MSVC_BUILTIN_BSWAP)
endif()

#set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# zlib
if(GMIO_USE_BUNDLED_ZLIB)
    message(STATUS "Bundled version of zlib")
    set(ZLIB_INCLUDE_DIRS
            ${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty/zlib
            ${CMAKE_CURRENT_BINARY_DIR}/src/3rdparty/zlib)
    set(ZLIB_LIBRARIES zlibstatic)
else()
    find_package(ZLIB)
    if (NOT(ZLIB_VERSION_MAJOR EQUAL 1) AND NOT(ZLIB_VERSION_MINOR EQUAL 2))
        message(FATAL_ERROR "Incompatible zlib version ${ZLIB_VERSION_STRING} (gmio expects v1.2.x)")
    endif()
endif()

# Sub-directories
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(doc)
if(GMIO_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
if(GMIO_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

message(STATUS "CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")
message(STATUS "GMIO_STR2FLOAT_LIBCODE = ${GMIO_STR2FLOAT_LIBCODE}(${GMIO_STR2FLOAT_LIB})")
message(STATUS "GMIO_FLOAT2STR_LIBCODE = ${GMIO_FLOAT2STR_LIBCODE}(${GMIO_FLOAT2STR_LIB})")

# Examples:
#   cmake -DCMAKE_INSTALL_PREFIX=gcc-linux64 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_DEBUG_POSTFIX=_d
#   cmake -DCMAKE_INSTALL_PREFIX=gcc-linux64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_RELEASE_POSTFIX=
# make VERBOSE=1  or  cmake -DCMAKE_VERBOSE_MAKEFILE=TRUE
