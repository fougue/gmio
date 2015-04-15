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
# TODO: check if GMIO_HAVE_STDINT_H is required or not
add_definitions(-DGMIO_HAVE_STDINT_H)

include_directories(${GMIO_DIR}/include)
link_directories(${GMIO_DIR}/lib)
link_libraries(gmio)
if(CMAKE_COMPILER_IS_GNUCC)
    link_libraries(m) # -lm
endif()

add_executable(bench_gmio bench_gmio/main.c ${COMMONS_FILES})

function(get_msvc_vernum outVerNum)
    if(MSVC60)
        set(${outVerNum} 6 PARENT_SCOPE)
    elseif(MSVC70)
        set(${outVerNum} 7 PARENT_SCOPE)
    elseif(MSVC80)
        set(${outVerNum} 8 PARENT_SCOPE)
    elseif(MSVC90)
        set(${outVerNum} 9 PARENT_SCOPE)
    elseif(MSVC10)
        set(${outVerNum} 10 PARENT_SCOPE)
    elseif(MSVC11)
        set(${outVerNum} 11 PARENT_SCOPE)
    elseif(MSVC12)
        set(${outVerNum} 12 PARENT_SCOPE)
    endif()
endfunction()

if(BENCHMARK_ASSIMP)
    add_executable(bench_assimp bench_assimp/main.cpp ${COMMONS_FILES})
    # Note: we could use target_include_directories() but it's available in cmake > v2.8.10
    set_property(
        TARGET bench_assimp
        APPEND PROPERTY INCLUDE_DIRECTORIES ${ASSIMP_DIR}/include)
    # Libs
    find_library(LIBPATH_ASSIMP assimp ${ASSIMP_DIR}/lib64)
    target_link_libraries(bench_assimp ${LIBPATH_ASSIMP})
endif()

if(BENCHMARK_OPENCASCADE)
    add_executable(bench_occ
        bench_occ/main.cpp
        ${GMIO_DIR}/src/gmio_support/stl_occ.cpp
        ${COMMONS_FILES})

    # TODO: define only if target arch is 64b
    # Noe: we could use target_compile_definitions() but it's available in cmake > v2.8.10
    set_property(
        TARGET bench_occ
        APPEND PROPERTY COMPILE_DEFINITIONS _OCC64)

    if(WIN32)
        set_property(
            TARGET bench_occ
            APPEND PROPERTY COMPILE_DEFINITIONS WNT)
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set_property(
            TARGET bench_occ
            APPEND PROPERTY COMPILE_DEFINITIONS
            HAVE_CONFIG_H  HAVE_FSTREAM  HAVE_IOSTREAM  HAVE_IOMANIP  HAVE_LIMITS_H)
    endif()

    # Note: we could use target_include_directories() but it's available in cmake > v2.8.10
    set_property(
        TARGET bench_occ
        APPEND PROPERTY INCLUDE_DIRECTORIES ${OPENCASCADE_DIR}/inc)

    # Libs
    if(MSVC)
        set(_MSVC_VERNUM_ 0) # Init
        get_msvc_vernum(_MSVC_VERNUM_)
        set(OPENCASCADE_LIBDIR ${OPENCASCADE_DIR}/win64/vc${_MSVC_VERNUM_}/lib)
    elseif((${CMAKE_SYSTEM_NAME} MATCHES "Linux") AND CMAKE_COMPILER_IS_GNUCXX)
        set(OPENCASCADE_LIBDIR ${OPENCASCADE_DIR}/lin64/gcc/lib)
    endif()
    message(STATUS ${OPENCASCADE_LIBDIR})

    find_library(LIBPATH_OPENCASCADE_TKERNEL TKernel ${OPENCASCADE_LIBDIR})
    find_library(LIBPATH_OPENCASCADE_TKMATH TKMath ${OPENCASCADE_LIBDIR})
    find_library(LIBPATH_OPENCASCADE_TKSTL TKSTL ${OPENCASCADE_LIBDIR})
    target_link_libraries(
        bench_occ
        ${LIBPATH_OPENCASCADE_TKERNEL}
        ${LIBPATH_OPENCASCADE_TKMATH}
        ${LIBPATH_OPENCASCADE_TKSTL})
endif()


# Find bit size of the target architecture
math(EXPR GMIO_TARGET_ARCH_BIT_SIZE "8 * ${CMAKE_SIZEOF_VOID_P}")


# Specific flags for Visual C++
if(MSVC)
    # Disable deprecation warnings about "non-secure" CRT functions
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
