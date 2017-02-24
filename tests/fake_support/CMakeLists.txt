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

file(GLOB  GMIO_FAKEOCC_HEADERS  opencascade/*.hxx  opencascade/*.h)
set(GMIO_FAKEOCC_HEADERS  ${GMIO_FAKEOCC_HEADERS})

add_executable(
    fake_support
    EXCLUDE_FROM_ALL
        main.cpp
        ${GMIO_FAKEOCC_HEADERS}
        qt/QtCore/QFile
        qt/QtCore/QIODevice
        qt/QtCore/QString
        qt/QtCore/QtGlobal
        ../../src/gmio_support/stl_occ_brep.cpp
        ../../src/gmio_support/stl_occ_mesh.cpp
        ../../src/gmio_support/stl_occ_meshvs.cpp
        ../../src/gmio_support/stl_occ_polytri.cpp
        ../../src/gmio_support/stream_qt.cpp)
target_link_libraries(fake_support gmio_static)
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/opencascade
    ${CMAKE_CURRENT_SOURCE_DIR}/qt)
