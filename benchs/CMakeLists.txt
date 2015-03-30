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

cmake_minimum_required(VERSION 2.6)

project(gmio_benchmark)

#set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Options
option(BENCHMARK_ASSIMP  "Build benchmark for Assimp" ON)
option(BENCHMARK_OPENCASCADE  "Build benchmark for OpenCascade" ON)

set(ASSIMP_DIR ${CMAKE_SOURCE_DIR} CACHE PATH "Directory where the Assimp library resides")
set(OPENCASCADE_DIR ${CMAKE_SOURCE_DIR} CACHE PATH "Directory where the OpenCascade library resides")
set(GMIO_DIR ${CMAKE_SOURCE_DIR} CACHE PATH "Directory where the GeomIO library resides")

# List source files in commons/
file(GLOB COMMONS_FILES commons/*)
set(COMMONS_FILES  ${COMMONS_FILES})
#add_definitions(-DGMIO_HAVE_STDINT_H)

if(BENCHMARK_ASSIMP)
    add_executable(bench_assimp bench_assimp/main.cpp ${COMMONS_FILES})
    target_include_directories(
        bench_assimp
        PUBLIC ${ASSIMP_DIR}/include
        PUBLIC ${GMIO_DIR}/include)
    target_link_libraries(
        bench_assimp
        ${ASSIMP_DIR}/lib64/assimp.lib
        ${GMIO_DIR}/lib/gmio.lib)
endif()

if(BENCHMARK_OPENCASCADE)
    add_executable(bench_occ
        bench_occ/main.cpp
        ${GMIO_DIR}/src/gmio_support/occ_libstl.cpp
        ${COMMONS_FILES})
    add_definitions(-D_OCC64 -DWNT)
    target_include_directories(
        bench_occ
        PUBLIC ${OPENCASCADE_DIR}/inc
        PUBLIC ${GMIO_DIR}/include)
    target_link_libraries(
        bench_occ
        ${OPENCASCADE_DIR}/win64/vc11/lib/TKernel.lib
        ${OPENCASCADE_DIR}/win64/vc11/lib/TKMath.lib
        ${OPENCASCADE_DIR}/win64/vc11/lib/TKSTL.lib
        ${GMIO_DIR}/lib/gmio.lib)
endif()


# Find bit size of the target architecture
math(EXPR GMIO_TARGET_ARCH_BIT_SIZE "8 * ${CMAKE_SIZEOF_VOID_P}")


# Specific flags for Visual C++
if(MSVC)
    # Disable deprecation warnings about "non-secure" CRT functions
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
