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

file(GLOB COMMONS_FILES commons/*)
set(COMMONS_FILES  ${COMMONS_FILES})

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_BINARY_DIR}/src/gmio_core) # For cmake generated headers
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
