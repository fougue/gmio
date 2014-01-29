cmake_minimum_required(VERSION 2.6)

include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckCSourceCompiles)

project(fougdatax)

# Options
option(BUILD_SHARED_LIBS       "Build shared libraries (DLL)" ON)
option(BUILD_STRICT_C90        "Build with strict conformance to language standard C90" ON)
option(BUILD_WITH_LIBSTL       "Build the libSTL module" ON)
option(BUILD_WITH_QT_SUPPORT   "Build with Qt support" OFF)
option(BUILD_WITH_OCC_SUPPORT  "Build with OpenCascade support" OFF)

# Add core source files
file(GLOB  ALL_SRC_FILES  src/*  src/internal/*)
set(ALL_SRC_FILES  ${ALL_SRC_FILES})

# Have <stdint.h> ?
check_include_files(stdint.h FOUG_HAVE_STDINT_H)

# Have strtof() ?
if(NOT BUILD_STRICT_C90)
  check_function_exists(strtof FOUG_HAVE_STRTOF_FUNC)
endif()

# Have builtin byte swap functions ?
if(CMAKE_COMPILER_IS_GNUCC)
  check_c_source_compiles("int main() { return (int)__builtin_bswap32(0x11223344); }"
                           FOUG_HAVE_GCC_BUILTIN_BSWAP_FUNC)
elseif(MSVC)
  check_c_source_compiles("#include <stdlib.h>
                           int main() { return (int)_byteswap_ulong(0x11223344); }"
                          FOUG_HAVE_MSVC_BUILTIN_BSWAP_FUNC)
endif()

configure_file(src/config.h.cmake config.h @ONLY)
include_directories(${CMAKE_BINARY_DIR}) # For generated "config.h"

# Specific flags for gcc
if(CMAKE_COMPILER_IS_GNUCC)
  if (BUILD_STRICT_C90)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi")
  endif()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic-errors -fstrict-aliasing")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Winline -Wextra -Wstrict-aliasing -Wcast-align -Wlogical-op -Wfloat-equal")
endif()

# Specific flags for Visual C++
if(MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -TC")
endif()

if(BUILD_SHARED_LIBS)
  add_definitions(-DFOUG_LIB_DLL
                  -DFOUG_LIB_MAKE_DLL)
endif()

# Declare installs
install(FILES ${CMAKE_BINARY_DIR}/config.h DESTINATION include/datax)

file(GLOB  C_GLOBAL_HEADERS  src/*.h)
install(FILES ${C_GLOBAL_HEADERS} DESTINATION include/datax)

# Module libSTL
if(BUILD_WITH_LIBSTL)
  add_definitions(-DFOUG_DATAX_LIBSTL_DLL
                  -DFOUG_DATAX_LIBSTL_MAKE_DLL)

  file(GLOB  ALL_LIBSTL_SRC_FILES  src/libstl/*  src/internal/libstl/*)
  set(ALL_SRC_FILES  ${ALL_SRC_FILES}  ${ALL_LIBSTL_SRC_FILES})

endif()
file(GLOB  C_LIBSTL_HEADERS  src/libstl/*.h)
install(FILES ${C_LIBSTL_HEADERS} DESTINATION include/datax/libstl)


# Common for support modules
if(BUILD_WITH_QT_SUPPORT OR BUILD_WITH_OCC_SUPPORT)
  if(BUILD_SHARED_LIBS)
    add_definitions(-DFOUG_LIBSUPPORT_DLL
                    -DFOUG_LIBSUPPORT_MAKE_DLL)
  endif()
endif()
install(FILES src/support/support_global.h DESTINATION include/datax/support)

# Qt support
if(BUILD_WITH_QT_SUPPORT)
  find_package(Qt5Core REQUIRED)
  file(GLOB  ALL_QT_SUPPORT_SRC_FILES  src/support/qt_*)
  set(ALL_SRC_FILES  ${ALL_SRC_FILES}  ${ALL_QT_SUPPORT_SRC_FILES})
else()
  install(FILES src/support/qt_stream.cpp DESTINATION src/support)
endif()
install(FILES src/support/qt_stream.h DESTINATION include/datax/support)

# OpenCASCADE support
if(BUILD_WITH_OCC_SUPPORT)
  set(CASCADE_ROOT "" CACHE PATH "Root directory of OpenCascade")

  file(GLOB  ALL_OCC_SUPPORT_SRC_FILES  src/support/occ_*)
  set(ALL_SRC_FILES  ${ALL_SRC_FILES}  ${ALL_OCC_SUPPORT_SRC_FILES})
  include_directories(${CASCADE_ROOT}/inc)

  # Defines
  if(WIN32)
    add_definitions(-DWNT) 
  elseif("${CMAKE_SYSTEM}" MATCHES "Linux")
    add_definitions(-DLIN
                    -DLININTEL
                    -DOCC_CONVERT_SIGNALS) 
  endif()

  if(CMAKE_SIZEOF_VOID_P EQUAL 8) # 64b ?
    add_definitions(-D_OCC64)
  endif()

  if(NOT WIN32)
    add_definitions(-DHAVE_CONFIG_H
                    -DHAVE_FSTREAM
                    -DHAVE_IOSTREAM
                    -DHAVE_IOMANIP
                    -DHAVE_LIMITS_H)
  endif()

  # Library path
  link_directories(${CASCADE_ROOT}/lib)
else()
  install(FILES src/support/occ_libstl.cpp DESTINATION src/support)
endif()
install(FILES src/support/occ_libstl.h DESTINATION include/datax/support)


# Declare fougdatax library
add_library(fougdatax ${ALL_SRC_FILES})
install(TARGETS fougdatax
          RUNTIME DESTINATION lib
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib)


# Qt support link
if(BUILD_WITH_QT_SUPPORT)
  qt5_use_modules(fougdatax Core)
endif()

# OpenCascade support ling
if(BUILD_WITH_OCC_SUPPORT)
  target_link_libraries(fougdatax TKSTL TKernel TKMath)  
endif()

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Examples:
#   cmake ../.. -DCMAKE_INSTALL_PREFIX=../../gcc-linux64 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_DEBUG_POSTFIX=.debug
#   cmake ../.. -DCMAKE_INSTALL_PREFIX=../../gcc-linux64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_RELEASE_POSTFIX=.release
# make VERBOSE=1  or  cmake -DCMAKE_VERBOSE_MAKEFILE=TRUE


# Tests

enable_testing()
set(CMAKE_CTEST_COMMAND ctest -V)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

add_executable(test_internal EXCLUDE_FROM_ALL tests/test_internal.c
                                              tests/stream_buffer.c
                                              src/stream.c
                                              src/internal/ascii_parse.c)
add_test(test_internal test_internal)
add_dependencies(check test_internal)

