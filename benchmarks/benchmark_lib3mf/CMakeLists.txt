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

set(ROOTDIR_LIB3MF ${CMAKE_SOURCE_DIR} CACHE PATH
    "Directory where the Lib3MF library resides")

if(CMAKE_C_COMPILER_IS_GCC_COMPATIBLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

include_directories(${ROOTDIR_LIB3MF}/Include)
add_executable(benchmark_lib3mf main.cpp ${COMMONS_FILES})

if(GMIO_TARGET_ARCH_BIT_SIZE EQUAL 64)
    set(LIB3MF_TARGET_ARCH "x64")
elseif(GMIO_TARGET_ARCH_BIT_SIZE EQUAL 32)
    set(LIB3MF_TARGET_ARCH "x86")
else()
    message(FATAL_ERROR "Target architecture not supported")
endif()

find_library(
    LIB_LIB3MF Lib3MF ${ROOTDIR_LIB3MF}/Project/Lib3MF/${LIB3MF_TARGET_ARCH}/${CMAKE_BUILD_TYPE}
    DOC "Path to the lib3mf import library")
target_link_libraries(benchmark_lib3mf ${LIB_LIB3MF})
