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

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_BINARY_DIR}/src/gmio_core) # For cmake generated headers
include_directories(${CMAKE_SOURCE_DIR}/tests/fake_support/opencascade)
link_libraries(gmio_static)
if(CMAKE_C_COMPILER_IS_GCC_COMPATIBLE)
    link_libraries(m) # -lm
endif()

file(GLOB SUPPORT_STL_OCC_FILES ${CMAKE_SOURCE_DIR}/src/gmio_support/stl_occ_mesh.*)
set(SUPPORT_STL_OCC_FILES_FILES ${SUPPORT_STL_OCC_FILES_FILES})

# gmio OpenCascade/STL support
add_executable(occstl_read_file
                   occstl_read_file.cpp ${SUPPORT_STL_OCC_FILES})
add_executable(occstl_write_file
                   occstl_write_file.cpp ${SUPPORT_STL_OCC_FILES})
add_executable(occstl_redefine_mesh_creator
                   occstl_redefine_mesh_creator.cpp ${SUPPORT_STL_OCC_FILES})

# gmio STL
add_executable(stl_get_infos  stl_get_infos.c)
add_executable(stl_read_file  stl_read_file.c)
add_executable(stl_write_file stl_write_file.c)
