#############################################################################
## Copyright (c) 2017, Fougue Ltd. <http://www.fougue.pro>
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
