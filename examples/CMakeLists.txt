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

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_BINARY_DIR}/src/gmio_core) # For cmake generated headers
include_directories(${CMAKE_SOURCE_DIR}/tests/fake_support/opencascade)
link_libraries(gmio)
if(CMAKE_C_COMPILER_IS_GCC_COMPATIBLE)
    link_libraries(m) # -lm
endif()

file(GLOB SUPPORT_STL_OCC_FILES ${CMAKE_SOURCE_DIR}/src/gmio_support/stl_occ_mesh.*)
set(SUPPORT_STL_OCC_FILES_FILES ${SUPPORT_STL_OCC_FILES_FILES})

# gmio OpenCascade/STL support
add_executable(occstl_read_file
                   occstl_read_file.cpp ${SUPPORT_STL_OCC_FILES})
add_executable(occstl_write_file
                   occstl_write_file.cpp ${SUPPORT_STL_OCC_FILES})
add_executable(occstl_redefine_mesh_creator
                   occstl_redefine_mesh_creator.cpp ${SUPPORT_STL_OCC_FILES})
if(CMAKE_COMPILER_IS_GNUCC)
    set_target_properties(
        occstl_read_file
        occstl_write_file
        occstl_redefine_mesh_creator
        PROPERTIES COMPILE_FLAGS "-std=c++0x")
endif()

# gmio STL
add_executable(stl_get_infos  stl_get_infos.c)
add_executable(stl_read_file  stl_read_file.c)
add_executable(stl_write_file stl_write_file.c)
