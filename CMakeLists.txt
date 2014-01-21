cmake_minimum_required(VERSION 2.6)
include(CheckIncludeFiles)

project(fougdatax)

# Options
option(BUILD_SHARED_LIBS "Build shared libraries (DLL)" ON)
option(WITH_LIBSTL       "Build the libSTL module" ON)
option(WITH_QT_SUPPORT   "Build with Qt support" OFF)
option(WITH_OCC_SUPPORT  "Build with OpenCascade support" OFF)

# Add core source files
file(GLOB  ALL_SRC_FILES  src/*)

# Have <stdint.h> ?
check_include_files(stdint.h FOUG_HAVE_STDINT_H)
configure_file(src/config.h.cmake config.h @ONLY)
include_directories(${CMAKE_BINARY_DIR}) # For generated "config.h"

# Specific flags for gcc
if(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic-errors -fstrict-aliasing")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra -Wstrict-aliasing -Wcast-align -Wlogical-op -Wfloat-equal")
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
if(WITH_LIBSTL)
  add_definitions(-DFOUG_DATAX_LIBSTL_DLL
                  -DFOUG_DATAX_LIBSTL_MAKE_DLL)

  file(GLOB  ALL_LIBSTL_SRC_FILES  src/libstl/*)
  set(ALL_SRC_FILES  ${ALL_SRC_FILES}  ${ALL_LIBSTL_SRC_FILES})

  file(GLOB  C_LIBSTL_HEADERS  src/libstl/*.h)
  install(FILES ${C_LIBSTL_HEADERS} DESTINATION include/datax/libstl)
endif()


# Common for support modules
if(WITH_QT_SUPPORT OR WITH_OCC_SUPPORT)
  if(BUILD_SHARED_LIBS)
    add_definitions(-DFOUG_LIBSUPPORT_DLL
                    -DFOUG_LIBSUPPORT_MAKE_DLL)
  endif()

  install(FILES src/support/support_global.h DESTINATION include/datax/support)
endif()

# Qt support
if(WITH_QT_SUPPORT)
  find_package(Qt5Core REQUIRED)

  file(GLOB  ALL_QT_SUPPORT_SRC_FILES  src/support/qt_*)
  set(ALL_SRC_FILES  ${ALL_SRC_FILES}  ${ALL_QT_SUPPORT_SRC_FILES})
  install(FILES src/support/qt_stream.h DESTINATION include/datax/support)
endif()

# OpenCASCADE support
if(WITH_OCC_SUPPORT)
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

  # Installs
  install(FILES src/support/occ_libstl.h DESTINATION include/datax/support)
endif()


# Declare fougdatax library
add_library(fougdatax ${ALL_SRC_FILES})
install(TARGETS fougdatax
          RUNTIME DESTINATION lib
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib)


# Qt support link
if(WITH_QT_SUPPORT)
  qt5_use_modules(fougdatax Core)
endif()

# OpenCascade support ling
if(WITH_OCC_SUPPORT)
  target_link_libraries(fougdatax TKSTL TKernel TKMath)  
endif()

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Examples:
#   cmake ../.. -DCMAKE_INSTALL_PREFIX=../../gcc-linux64 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_DEBUG_POSTFIX=.debug
#   cmake ../.. -DCMAKE_INSTALL_PREFIX=../../gcc-linux64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_RELEASE_POSTFIX=.release
# make VERBOSE=1  or  cmake -DCMAKE_VERBOSE_MAKEFILE=TRUE
