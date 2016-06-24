#############################################################################
## gmio
## Copyright Fougue (24 Jun. 2016)
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
link_libraries(gmio)
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
