#############################################################################
## gmio
## Copyright Fougue (2 Mar. 2015)
## contact@fougue.pro
##
## This software is a reusable library whose purpose is to provide complete
## I/O support for various CAD file formats (eg. STL)
##
## This software is governed by the CeCILL-B license under French law and
## abiding by the rules of distribution of free software.  You can  use,
## modify and/ or redistribute the software under the terms of the CeCILL-B
## license as circulated by CEA, CNRS and INRIA at the following URL
## "http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html".
#############################################################################

cmake_minimum_required(VERSION 2.8)

include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckCSourceCompiles)
include(CheckTypeSize)

project(gmio)
set(GMIO_VERSION_MAJOR 0)
set(GMIO_VERSION_MINOR 1)
set(GMIO_VERSION_PATCH 0)
set(GMIO_VERSION
    ${GMIO_VERSION_MAJOR}.${GMIO_VERSION_MINOR}.${GMIO_VERSION_PATCH})

# Options
option(GMIO_BUILD_STRICT_C90  "Build gmio library(and tests) with strict conformance to C90 standard" OFF)
option(GMIO_BUILD_SHARED_LIBS "Build gmio as a shared library (DLL)" OFF)
option(GMIO_BUILD_BENCHMARKS  "Build performance benchmarks for the gmio library" OFF)
if(GMIO_BUILD_BENCHMARKS)
    option(GMIO_BUILD_BENCHMARK_ASSIMP      "Build benchmark for Assimp" ON)
    option(GMIO_BUILD_BENCHMARK_OPENCASCADE "Build benchmark for OpenCascade" ON)
endif()

# Detect Clang
if (CMAKE_C_COMPILER MATCHES ".*clang")
    set(CMAKE_COMPILER_IS_CLANG 1)
endif ()

# Find bit size of the target architecture
math(EXPR GMIO_TARGET_ARCH_BIT_SIZE "8 * ${CMAKE_SIZEOF_VOID_P}")

# Test if host's architecture is big-endian
include(TestBigEndian)
test_big_endian(GMIO_HOST_IS_BIG_ENDIAN)

# Adapt C compiler flags to satisfy the GMIO_BUILD_STRICT_C90 option
#    It must be done before checking of C99 and POSIX features
if(GMIO_BUILD_STRICT_C90)
    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic-errors")
    elseif(MSVC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Za")
    endif()
endif()

set(CMAKE_REQUIRED_FLAGS "${CMAKE_C_FLAGS}")

# Some compilers (like GCC v4.9) don't disable <stdint.h> in STRICT_ANSI mode
check_include_files(stdint.h GMIO_HAVE_STDINT_H)

# Check non-ANSI available features
if(NOT GMIO_BUILD_STRICT_C90)
    # Check C99 features
    check_function_exists(strtof GMIO_HAVE_STRTOF_FUNC)

    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG)
        list(APPEND CMAKE_REQUIRED_LIBRARIES m) # -lm
    endif()
    check_function_exists(powf GMIO_HAVE_POWF_FUNC)
    set(CMAKE_REQUIRED_LIBRARIES)

    check_include_files(stdbool.h GMIO_HAVE_STDBOOL_H)

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

    check_include_files(sys/types.h GMIO_HAVE_SYS_TYPES_H)
    check_include_files(sys/stat.h GMIO_HAVE_SYS_STAT_H)
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

    # Verify large file support
    if(GMIO_HAVE_SYS_STAT_H
           AND NOT(GMIO_SIZEOF_STRUCT_STAT_ST_SIZE STREQUAL "0") # Not arch-dependent size
           AND NOT(GMIO_SIZEOF_STRUCT_STAT_ST_SIZE EQUAL 8))
        message(WARNING "<sys/stat.h> does not provide 64b variant of fstat(), you may encounter problems with files > 2GB")
    endif()

    # Have builtin byte swap functions ?
    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG)
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
endif() # !GMIO_BUILD_STRICT_C90

# Specific flags for GCC and Clang
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstrict-aliasing")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wstrict-aliasing")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wcast-align -Wfloat-equal")
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Winline")
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-field-initializers")
    if(CMAKE_COMPILER_IS_GNUCC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wlogical-op")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wlogical-op-parentheses")
    endif()

    # Disable some warnings
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-missing-braces -Wno-missing-field-initializers")

    # Force PIC for GCC, see : https://www.gentoo.org/proj/en/base/amd64/howtos/index.xml?part=1&chap=3
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Specific flags for Visual C++
if(MSVC)
    # Treat all files a C source files
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /TC")

    # Set warning level to /W4
    string(REGEX REPLACE "/W[0-9]" "/W4" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /analyze")

    # Treat warnings as errors
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /WX")

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

# Examples:
#   cmake ../.. -DCMAKE_INSTALL_PREFIX=../../gcc-linux64 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_DEBUG_POSTFIX=.debug
#   cmake ../.. -DCMAKE_INSTALL_PREFIX=../../gcc-linux64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_RELEASE_POSTFIX=.release
# make VERBOSE=1  or  cmake -DCMAKE_VERBOSE_MAKEFILE=TRUE
