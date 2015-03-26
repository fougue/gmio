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
## "http://www.cecill.info".
#############################################################################

cmake_minimum_required(VERSION 2.6)

include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckCSourceCompiles)

project(gmio C)

#set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Options
option(BUILD_SHARED_LIBS  "Build shared libraries (DLL)" ON)
option(BUILD_STRICT_C90   "Build with strict conformance to C90 standard. If disabled, C99 features can be used (eg. strtof(), <stdint.h>, ...)" ON)
option(BUILD_WITH_LIBSTL  "Build the libSTL module" ON)

# Add core source files
file(GLOB  ALL_SRC_FILES  src/gmio_core/*  src/gmio_core/internal/*)
set(ALL_SRC_FILES  ${ALL_SRC_FILES})

# Find bit size of the target architecture
math(EXPR GMIO_TARGET_ARCH_BIT_SIZE "8 * ${CMAKE_SIZEOF_VOID_P}")

# Test if host's architecture is big-endian
include(TestBigEndian)
test_big_endian(GMIO_HOST_IS_BIG_ENDIAN)

# Find available C99 features
if(NOT BUILD_STRICT_C90)
    # Have <stdint.h> ?
    check_include_files(stdint.h GMIO_HAVE_STDINT_H)

    # Have strtof() ?
    check_function_exists(strtof GMIO_HAVE_STRTOF_FUNC)

    # Have powf() ?
    check_function_exists(powf GMIO_HAVE_POWF_FUNC)

    # Have <stdbool.h> ?
    check_include_files(stdbool.h GMIO_HAVE_STDBOOL_H)
endif()

# Have builtin byte swap functions ?
if(CMAKE_COMPILER_IS_GNUCC)
    # __builtin_bswap16() is missing in x86 GCC version prior to v4.7
    # See http://gcc.gnu.org/bugzilla/show_bug.cgi?id=52624
    check_c_source_compiles("int main() { return (int)__builtin_bswap16(0x1122); }"
                             GMIO_HAVE_GCC_BUILTIN_BSWAP16_FUNC)
    check_c_source_compiles("int main() { return (int)__builtin_bswap32(0x11223344); }"
                             GMIO_HAVE_GCC_BUILTIN_BSWAP32_FUNC)
elseif(MSVC)
    check_c_source_compiles("#include <stdlib.h>
                             int main() { return (int)_byteswap_ulong(0x11223344); }"
                            GMIO_HAVE_MSVC_BUILTIN_BSWAP_FUNC)
endif()

configure_file(src/gmio_core/config.h.cmake config.h @ONLY)
include_directories(${CMAKE_BINARY_DIR}) # For generated "config.h"

# Specific flags for GCC
if(CMAKE_COMPILER_IS_GNUCC)
    if(BUILD_STRICT_C90)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi")
    endif()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic-errors -fstrict-aliasing")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Winline -Wextra -Wstrict-aliasing -Wcast-align -Wlogical-op -Wfloat-equal")

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
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /WX")

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
install(FILES src/gmio_support/qt_stream.h   DESTINATION include/gmio_support)
install(FILES src/gmio_support/qt_stream.cpp DESTINATION src/gmio_support)

# OpenCASCADE support
install(FILES src/gmio_support/occ_libstl.h   DESTINATION include/gmio_support)
install(FILES src/gmio_support/occ_libstl.cpp DESTINATION src/gmio_support)

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
                                              tests/test_internal.c
                                              src/gmio_core/stream.c
                                              src/gmio_core/internal/string_parse.c)
add_executable(test_platform EXCLUDE_FROM_ALL tests/test_platform.c)

add_test(test_internal test_internal)
add_test(test_platform test_platform)

add_dependencies(check  test_internal
                        test_platform)
