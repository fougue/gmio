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
        ../../src/gmio_support/stl_occ_mesh.cpp
        ../../src/gmio_support/stl_occ_meshvs.cpp
        ../../src/gmio_support/stl_occ_brep.cpp
        ../../src/gmio_support/stream_qt.cpp)
target_link_libraries(fake_support gmio)
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/opencascade
    ${CMAKE_CURRENT_SOURCE_DIR}/qt)

if(CMAKE_COMPILER_IS_GNUCC)
    set_target_properties(
        fake_support
        PROPERTIES COMPILE_FLAGS "-std=c++0x")
endif()
