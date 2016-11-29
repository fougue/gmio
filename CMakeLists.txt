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

cmake_minimum_required(VERSION 2.8)

include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckSymbolExists)
include(CheckCSourceCompiles)
include(CheckTypeSize)
include(CMakeDependentOption)

project(gmio)
set(GMIO_VERSION_MAJOR 0)
set(GMIO_VERSION_MINOR 4)
set(GMIO_VERSION_PATCH 0)
set(GMIO_VERSION
    ${GMIO_VERSION_MAJOR}.${GMIO_VERSION_MINOR}.${GMIO_VERSION_PATCH})

# C Compiler detection
if(CMAKE_C_COMPILER MATCHES ".*clang")
    set(CMAKE_COMPILER_IS_CLANG 1)
endif ()

if(CMAKE_C_COMPILER MATCHES "tcc")
    set(CMAKE_COMPILER_IS_TCC 1) # TinyCC
endif()

if(CMAKE_COMPILER_IS_GNUCC
       OR CMAKE_COMPILER_IS_CLANG
       OR CMAKE_COMPILER_IS_TCC)
    set(CMAKE_C_COMPILER_IS_GCC_COMPATIBLE 1) # Compatible with GNUCC options
endif()

# Build type
string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if(CMAKE_BUILD_TYPE_LOWER MATCHES "debug")
    set(GMIO_DEBUG_BUILD 1)
else()
    set(GMIO_DEBUG_BUILD 0)
endif()

# Options
option(GMIO_BUILD_STRICT_C90  "Build gmio library(and tests) with strict conformance to C90 standard" OFF)
option(GMIO_BUILD_SHARED_LIBS "Build gmio as a shared library (DLL)" OFF)
option(GMIO_BUILD_BENCHMARKS  "Build performance benchmarks for the gmio library" OFF)
option(GMIO_BUILD_EXAMPLES    "Build gmio examples" OFF)
option(GMIO_BUILD_TESTS_FAKE_SUPPORT  "Build tests/fake_support target" OFF)
if(CMAKE_C_COMPILER_IS_GCC_COMPATIBLE)
    option(GMIO_BUILD_TESTS_COVERAGE "Instrument testing code with code coverage" OFF)
endif()

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

# Adapt C compiler flags to satisfy the GMIO_BUILD_STRICT_C90 option
#    It must be done before checking non ANSI features(C99, POSIX, ...)
if(GMIO_BUILD_STRICT_C90)
    if(CMAKE_C_COMPILER_IS_GCC_COMPATIBLE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic-errors")
    elseif(MSVC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Za")
    endif()
endif()

set(CMAKE_REQUIRED_FLAGS "${CMAKE_C_FLAGS}")

# Some compilers (like GCC v4.9) don't disable <stdint.h> in C90 mode
check_include_files(stdint.h GMIO_HAVE_STDINT_H)

# Find size of short, int and long types
set(GMIO_SIZEOF_SHORT ${CMAKE_SIZEOF_UNSIGNED_SHORT})
if (NOT DEFINED HAVE_CMAKE_SIZEOF_UNSIGNED_SHORT)
    check_type_size("short" GMIO_SIZEOF_SHORT)
endif()
check_type_size("int" GMIO_SIZEOF_INT)
check_type_size("long" GMIO_SIZEOF_LONG)

# Check availability of a 64b integer type
if(GMIO_HAVE_STDINT_H)
    set(CMAKE_REQUIRED_INCLUDES stdint.h)
endif()
check_type_size("int64_t" GMIO_SIZEOF_INT64_T)
if(NOT HAVE_GMIO_SIZEOF_INT64_T)
    check_type_size("__int64" GMIO_SIZEOF_MSVC_INT64)
    if(NOT HAVE_GMIO_SIZEOF_MSVC_INT64)
        check_type_size("long long" GMIO_SIZEOF_LONG_LONG)
    endif()
endif()
set(CMAKE_REQUIRED_INCLUDES) # Pop changes

if(HAVE_GMIO_SIZEOF_INT64_T)
    set(GMIO_HAVE_INT64_T ${HAVE_GMIO_SIZEOF_INT64_T})
elseif(HAVE_GMIO_SIZEOF_MSVC_INT64)
    set(GMIO_HAVE_MSVC_INT64 ${HAVE_GMIO_SIZEOF_MSVC_INT64})
elseif(HAVE_GMIO_SIZEOF_LONG_LONG)
    set(GMIO_HAVE_LONG_LONG ${HAVE_GMIO_SIZEOF_LONG_LONG})
endif()

# Check C99 features
if(NOT GMIO_BUILD_STRICT_C90)
    if(CMAKE_C_COMPILER_IS_GCC_COMPATIBLE)
        list(APPEND CMAKE_REQUIRED_LIBRARIES m) # -lm
    endif()
    check_function_exists(powf  GMIO_HAVE_POWF_FUNC)
    check_function_exists(sqrtf GMIO_HAVE_SQRTF_FUNC)
    check_function_exists(strtof GMIO_HAVE_STRTOF_FUNC)
    check_symbol_exists(isfinite math.h GMIO_HAVE_ISFINITE_MACRO)
    check_symbol_exists(isnan    math.h GMIO_HAVE_ISNAN_MACRO)
    set(CMAKE_REQUIRED_LIBRARIES) # Pop changes

    check_include_files(stdbool.h GMIO_HAVE_STDBOOL_H)

    check_function_exists(snprintf GMIO_HAVE_SNPRINTF_FUNC)
    if(WIN32 AND NOT GMIO_HAVE_SNPRINTF_FUNC)
        check_function_exists(_snprintf GMIO_HAVE_WIN__SNPRINTF_FUNC)
    endif()

    check_function_exists(vsnprintf GMIO_HAVE_VSNPRINTF_FUNC)
    if(WIN32 AND NOT GMIO_HAVE_VSNPRINTF_FUNC)
        check_function_exists(_vsnprintf GMIO_HAVE_WIN__VSNPRINTF_FUNC)
    endif()
endif()

# Check POSIX features
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

check_function_exists(fileno GMIO_HAVE_POSIX_FILENO_FUNC)

# Have fstat64() ?
check_c_source_compiles(
    "#include <sys/stat.h>
     int main() { fstat64(0, NULL); return 0; }"
    GMIO_HAVE_POSIX_FSTAT64_FUNC)
if(WIN32)
    check_function_exists(_fstat64 GMIO_HAVE_WIN__FSTAT64_FUNC)
endif()

# Check size(in bytes) of stat::st_size
set(CMAKE_EXTRA_INCLUDE_FILES sys/stat.h)
if(GMIO_HAVE_WIN__FSTAT64_FUNC)
    check_type_size("((struct _stat64*)0)->st_size" GMIO_SIZEOF_STRUCT_STAT_ST_SIZE)
elseif(GMIO_HAVE_POSIX_FSTAT64_FUNC)
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
if(CMAKE_C_COMPILER_IS_GCC_COMPATIBLE)
    # __builtin_bswap16() is missing in x86 GCC version prior to v4.7
    # See http://gcc.gnu.org/bugzilla/show_bug.cgi?id=52624
    check_c_source_compiles(
        "int main() { return (int)__builtin_bswap16(0x1122); }"
        GMIO_HAVE_GCC_BUILTIN_BSWAP16_FUNC)
    check_c_source_compiles(
        "int main() { return (int)__builtin_bswap32(0x11223344); }"
        GMIO_HAVE_GCC_BUILTIN_BSWAP32_FUNC)
elseif(MSVC)
    check_c_source_compiles(
        "#include <stdlib.h>
         int main() { return (int)_byteswap_ulong(0x11223344); }"
        GMIO_HAVE_MSVC_BUILTIN_BSWAP_FUNC)
endif()

# Specific flags for GCC-like compilers
if(CMAKE_C_COMPILER_IS_GCC_COMPATIBLE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstrict-aliasing")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wstrict-aliasing")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wcast-align -Wfloat-equal")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Winline")
    if(CMAKE_COMPILER_IS_GNUCC)
        # Undefined behavior sanitizer(ubsan), requires GCC >= v4.9
        #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wlogical-op")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wlogical-op-parentheses")
    endif()

    # Disable warnings -Wno-missing-braces -Wno-missing-field-initializers
    # in case GCC wrongly warns about universal zero initializer {0}
    # See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53119
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_C_FLAGS} -Werror=missing-braces")
    check_c_source_compiles(
        "struct data { char array[128]; };
         int main() {
             struct data d = {0};
             return d.array[0];
         }"
        GMIO_GCC_DOESNT_WARN_UNIVERSAL_0_INITIALIZER)
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_C_FLAGS}") # Restore CMAKE_REQUIRED_FLAGS
    if (NOT GMIO_GCC_DOESNT_WARN_UNIVERSAL_0_INITIALIZER)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-missing-braces -Wno-missing-field-initializers")
    endif()

    # Force PIC for GCC
    #     See https://www.gentoo.org/proj/en/base/amd64/howtos/index.xml?part=1&chap=3
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Specific flags for Clang
if(CMAKE_COMPILER_IS_CLANG)
    # Clang on apple-darwin13.4.0 wrongly reports many unused functions
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-function")
endif()

# Specific flags for Visual C++
if(MSVC)
    # Treat all files a C source files
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /TC")

    # Set warning level to /W4
    string(REGEX REPLACE "/W[0-9]" "/W4" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})

    # Set warning level to /Wall
    #string(REGEX REPLACE "/W[0-9]" "/Wall" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4255 /wd4710 /wd4711 /wd4820")

    # Enable static analysis
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /analyze")
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Oi /Qvec-report:2")

    # Treat some warnings as errors
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /we4133")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /we4013")

    # Enable /Zc:strictStrings when msvc_ver > 2012 and build_type != Debug
    if((MSVC_VERSION GREATER 1700) AND NOT (CMAKE_BUILD_TYPE STREQUAL "Debug"))
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Zc:strictStrings")
    endif()

    # Disable deprecation warnings about "non-secure" CRT functions
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS)

    # Enable "Generate Intrinsic Functions"
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Oi")
endif()

#set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

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

message(STATUS "CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}")
message(STATUS "GMIO_STR2FLOAT_LIBCODE = ${GMIO_STR2FLOAT_LIBCODE}(${GMIO_STR2FLOAT_LIB})")
message(STATUS "GMIO_FLOAT2STR_LIBCODE = ${GMIO_FLOAT2STR_LIBCODE}(${GMIO_FLOAT2STR_LIB})")

# Examples:
#   cmake -DCMAKE_INSTALL_PREFIX=gcc-linux64 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_DEBUG_POSTFIX=_d
#   cmake -DCMAKE_INSTALL_PREFIX=gcc-linux64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_RELEASE_POSTFIX=
# make VERBOSE=1  or  cmake -DCMAKE_VERBOSE_MAKEFILE=TRUE
