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

cmake_dependent_option(
    GMIO_BUILD_BENCHMARK_ASSIMP "Build benchmark for Assimp" ON
    "GMIO_BUILD_BENCHMARKS" OFF)
cmake_dependent_option(
    GMIO_BUILD_BENCHMARK_OPENCASCADE "Build benchmark for OpenCascade" ON
    "GMIO_BUILD_BENCHMARKS" OFF)
if(WIN32)
    cmake_dependent_option(
        GMIO_BUILD_BENCHMARK_LIB3MF "Build benchmark for Lib3MF" ON
        "GMIO_BUILD_BENCHMARKS" OFF)
endif()

file(GLOB COMMONS_FILES commons/*)
set(COMMONS_FILES  ${COMMONS_FILES})

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_BINARY_DIR}/src) # For cmake generated headers
include_directories(${CMAKE_BINARY_DIR}/src/gmio_core)
link_libraries(gmio_static)
if(CMAKE_C_COMPILER_IS_GCC_COMPATIBLE)
    link_libraries(m) # -lm
endif()

add_subdirectory(benchmark_gmio)

if(GMIO_BUILD_BENCHMARK_ASSIMP)
    add_subdirectory(benchmark_assimp)
endif()

if(GMIO_BUILD_BENCHMARK_OPENCASCADE)
    add_subdirectory(benchmark_opencascade)
endif()

if(GMIO_BUILD_BENCHMARK_LIB3MF)
    add_subdirectory(benchmark_lib3mf)
endif()

add_subdirectory(benchmark_other)
