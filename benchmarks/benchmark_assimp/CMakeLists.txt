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

set(ROOTDIR_ASSIMP ${CMAKE_SOURCE_DIR} CACHE PATH
    "Directory where the Assimp library resides")

include_directories(${ROOTDIR_ASSIMP}/include)
add_executable(benchmark_assimp main.cpp ${COMMONS_FILES})

# Choose lib32/lib64 depending on the architecture
find_library(
    LIB_ASSIMP assimp ${ROOTDIR_ASSIMP}/lib${GMIO_TARGET_ARCH_BIT_SIZE}
    DOC "Path to the assimp import library")

target_link_libraries(benchmark_assimp ${LIB_ASSIMP})
