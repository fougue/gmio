cmake_minimum_required(VERSION 2.6)
include(CheckIncludeFiles)

project(foug_datax C)

# Allow the user to build a static library
option(BUILD_SHARED_LIBS "Build shared libraries (DLL)" ON)

# Declare fougstl-c library 
file(GLOB_RECURSE  ALL_SRC_FILES  src/c/*)
add_library(fougstl-c ${ALL_SRC_FILES})

# Have <stdint.h> ?
check_include_files(stdint.h HAVE_STDINT_H)
configure_file(src/c/config.h.cmake config.h @ONLY)
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
                  -DFOUG_LIB_MAKE_DLL
                  -DFOUG_DATAX_LIBSTL_DLL
                  -DFOUG_DATAX_LIBSTL_MAKE_DLL)
endif()

# Declare installs
install(FILES ${CMAKE_BINARY_DIR}/config.h DESTINATION include/datax/c)

file(GLOB  C_GLOBAL_HEADERS  src/c/*.h)
install(FILES ${C_GLOBAL_HEADERS} DESTINATION include/datax/c)

file(GLOB  C_LIBSTL_HEADERS  src/c/libstl/*.h)
install(FILES ${C_LIBSTL_HEADERS} DESTINATION include/datax/c/libstl)

install(TARGETS fougstl-c
          RUNTIME DESTINATION lib
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib
)


# Examples:
#   cmake ../.. -DCMAKE_INSTALL_PREFIX=../../gcc-linux64 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_DEBUG_POSTFIX=.debug
#   cmake ../.. -DCMAKE_INSTALL_PREFIX=../../gcc-linux64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_RELEASE_POSTFIX=.release
# make VERBOSE=1  or  cmake -DCMAKE_VERBOSE_MAKEFILE=TRUE
