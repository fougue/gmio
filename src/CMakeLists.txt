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

if(GMIO_BUILD_TESTS_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
    link_libraries(gcov)
endif()

# Add core source files
file(GLOB  GMIO_SRC_FILES  gmio_core/*  gmio_core/internal/*)
if(GMIO_STR2FLOAT_LIBCODE EQUAL 2 OR GMIO_FLOAT2STR_LIBCODE EQUAL 2)
    file(GLOB  GMIO_3RDPARTY_FILES  3rdparty/double-conversion/*)
endif()
set(GMIO_SRC_FILES  ${GMIO_SRC_FILES} ${GMIO_3RDPARTY_FILES})

configure_file(gmio_core/version.h.cmake gmio_core/version.h @ONLY)
configure_file(gmio_core/config.h.cmake gmio_core/config.h @ONLY)
include_directories(${CMAKE_BINARY_DIR}/src/gmio_core) # For cmake generated headers

# Enable DLL generation (export symbols)
if(GMIO_BUILD_SHARED_LIBS)
    add_definitions(-DGMIO_DLL
                    -DGMIO_MAKING_DLL)
    if(MSVC)
        configure_file(gmio_core/gmio.rc.cmake gmio_core/gmio.rc @ONLY)
        set(GMIO_SRC_FILES ${GMIO_SRC_FILES} ${CMAKE_BINARY_DIR}/src/gmio_core/gmio.rc)
    endif()
endif()

# Declare installs
install(FILES ${CMAKE_BINARY_DIR}/src/gmio_core/version.h DESTINATION include/gmio_core)
install(FILES ${CMAKE_BINARY_DIR}/src/gmio_core/config.h DESTINATION include/gmio_core)

file(GLOB  GMIO_CORE_HEADERS  gmio_core/*.h)
install(FILES ${GMIO_CORE_HEADERS} DESTINATION include/gmio_core)

# Module libSTL
#if(GMIO_BUILD_SHARED_LIBS)
#    add_definitions(-DGMIO_STL_DLL
#                    -DGMIO_STL_MAKING_DLL)
#endif()

file(GLOB  GMIO_LIBSTL_SRC_FILES  gmio_stl/*  gmio_stl/internal/*)
set(GMIO_SRC_FILES  ${GMIO_SRC_FILES}  ${GMIO_LIBSTL_SRC_FILES})

file(GLOB  GMIO_LIBSTL_HEADERS  gmio_stl/*.h)
install(FILES ${GMIO_LIBSTL_HEADERS} DESTINATION include/gmio_stl)


# Common for support modules
install(FILES gmio_support/support_global.h DESTINATION include/gmio_support)

# Qt support
install(FILES gmio_support/stream_qt.h   DESTINATION include/gmio_support)
install(FILES gmio_support/stream_qt.cpp DESTINATION src/gmio_support)

# OpenCASCADE support
install(FILES gmio_support/stl_occ_brep.h   DESTINATION include/gmio_support)
install(FILES gmio_support/stl_occ_mesh.h   DESTINATION include/gmio_support)
install(FILES gmio_support/stl_occ_meshvs.h DESTINATION include/gmio_support)
install(FILES gmio_support/stl_occ_brep.cpp   DESTINATION src/gmio_support)
install(FILES gmio_support/stl_occ_mesh.cpp   DESTINATION src/gmio_support)
install(FILES gmio_support/stl_occ_meshvs.cpp DESTINATION src/gmio_support)
install(FILES gmio_support/stl_occ_utils.h    DESTINATION src/gmio_support)

# Installs for target
if(GMIO_BUILD_SHARED_LIBS)
    add_library(gmio SHARED ${GMIO_SRC_FILES})
else()
    add_library(gmio STATIC ${GMIO_SRC_FILES})
endif()
    
install(TARGETS gmio
          RUNTIME DESTINATION lib
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib)
