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

set(ROOTDIR_ASSIMP ${CMAKE_SOURCE_DIR} CACHE PATH
    "Directory where the Assimp library resides")

include_directories(${ROOTDIR_ASSIMP}/include)
add_executable(benchmark_assimp main.cpp ${COMMONS_FILES})

# Choose lib32/lib64 depending on the architecture
find_library(
    LIB_ASSIMP assimp ${ROOTDIR_ASSIMP}/lib${GMIO_TARGET_ARCH_BIT_SIZE}
    DOC "Path to the assimp import library")

target_link_libraries(benchmark_assimp ${LIB_ASSIMP})
