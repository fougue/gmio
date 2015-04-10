#############################################################################
## GeomIO Library
## Copyright FougSys (2 Mar. 2015)
## contact@fougsys.fr
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

project(gmio C)

#set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Options
option(BUILD_SHARED_LIBS  "Build shared libraries (DLL)" OFF)
option(BUILD_STRICT_C90   "Build with strict conformance to C90 standard" OFF)
option(BUILD_WITH_LIBSTL  "Build the libSTL module" ON)

# Add core source files
file(GLOB  ALL_SRC_FILES  src/gmio_core/*  src/gmio_core/internal/*)
set(ALL_SRC_FILES  ${ALL_SRC_FILES})

# Find bit size of the target architecture
math(EXPR GMIO_TARGET_ARCH_BIT_SIZE "8 * ${CMAKE_SIZEOF_VOID_P}")

# Test if host's architecture is big-endian
include(TestBigEndian)
test_big_endian(GMIO_HOST_IS_BIG_ENDIAN)

# Adapt C compiler flags to satisfy the BUILD_STRICT_C90 option
#    It must be done before checking of C99 and POSIX features
if(BUILD_STRICT_C90)
    if(CMAKE_COMPILER_IS_GNUCC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic-errors")
    elseif(MSVC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Za")
    endif()
endif()

set(CMAKE_REQUIRED_FLAGS "${CMAKE_C_FLAGS}")

# Some compilers (like GCC v4.9) don't disable <stdint.h> in STRICT_ANSI mode
check_include_files(stdint.h GMIO_HAVE_STDINT_H)

# Check non-ANSI available features
if(NOT BUILD_STRICT_C90)
    # Check C99 features
    check_function_exists(strtof GMIO_HAVE_STRTOF_FUNC)

    if(CMAKE_COMPILER_IS_GNUCC)
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
    if(CMAKE_COMPILER_IS_GNUCC)
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
endif() # !BUILD_STRICT_C90

configure_file(src/gmio_core/config.h.cmake config.h @ONLY)
include_directories(${CMAKE_BINARY_DIR}) # For generated "config.h"

# Specific flags for GCC
if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstrict-aliasing")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wstrict-aliasing")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wcast-align -Wlogical-op -Wfloat-equal")
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Winline")
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-field-initializers")

    # Disable some warnings
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-missing-braces -Wno-missing-field-initializers")

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
endif()

# Enable DLL generation (export symbols)
if(BUILD_SHARED_LIBS)
    add_definitions(-DGMIO_LIB_DLL
                    -DGMIO_LIB_MAKE_DLL)
endif()

# Declare installs
install(FILES ${CMAKE_BINARY_DIR}/config.h DESTINATION include/gmio_core)

file(GLOB  C_CORE_HEADERS  src/gmio_core/*.h)
install(FILES ${C_CORE_HEADERS} DESTINATION include/gmio_core)

# Module libSTL
if(BUILD_WITH_LIBSTL)
    if(BUILD_SHARED_LIBS)
        add_definitions(-DGMIO_LIBSTL_DLL
                        -DGMIO_LIBSTL_MAKE_DLL)
    endif()

    file(GLOB  ALL_LIBSTL_SRC_FILES  src/gmio_stl/*  src/gmio_stl/internal/*)
    set(ALL_SRC_FILES  ${ALL_SRC_FILES}  ${ALL_LIBSTL_SRC_FILES})

endif()
file(GLOB  C_LIBSTL_HEADERS  src/gmio_stl/*.h)
install(FILES ${C_LIBSTL_HEADERS} DESTINATION include/gmio_stl)


# Common for support modules
install(FILES src/gmio_support/support_global.h DESTINATION include/gmio_support)

# Qt support
install(FILES src/gmio_support/stream_qt.h   DESTINATION include/gmio_support)
install(FILES src/gmio_support/stream_qt.cpp DESTINATION src/gmio_support)

# OpenCASCADE support
install(FILES src/gmio_support/stl_occ.h   DESTINATION include/gmio_support)
install(FILES src/gmio_support/stl_occ.cpp DESTINATION src/gmio_support)

# Installs for target
add_library(gmio ${ALL_SRC_FILES})
install(TARGETS gmio
          RUNTIME DESTINATION lib
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib)


# Examples:
#   cmake ../.. -DCMAKE_INSTALL_PREFIX=../../gcc-linux64 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_DEBUG_POSTFIX=.debug
#   cmake ../.. -DCMAKE_INSTALL_PREFIX=../../gcc-linux64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_RELEASE_POSTFIX=.release
# make VERBOSE=1  or  cmake -DCMAKE_VERBOSE_MAKEFILE=TRUE


# Tests

enable_testing()
set(CMAKE_CTEST_COMMAND ctest -V)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

add_executable(test_internal EXCLUDE_FROM_ALL tests/stream_buffer.c
                                              tests/utils.c
                                              tests/test_internal.c
                                              src/gmio_core/stream.c
                                              src/gmio_core/internal/string_parse.c)
add_executable(test_platform EXCLUDE_FROM_ALL tests/test_platform.c)

if(CMAKE_COMPILER_IS_GNUCC)
    target_link_libraries(test_internal m) # -lm
endif()

add_test(test_internal test_internal)
add_test(test_platform test_platform)

add_dependencies(check  test_internal
                        test_platform)
