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

enable_testing()
set(CMAKE_CTEST_COMMAND ctest -V)
if(WIN32 AND GMIO_BUILD_SHARED_LIBS)
    configure_file(win_ctest_cmd.bat.cmake win_ctest_cmd.bat @ONLY)
    set(CMAKE_CTEST_COMMAND win_ctest_cmd.bat)
endif()
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

include_directories(${CMAKE_BINARY_DIR}/src/gmio_core) # For generated cmake headers

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/temp)

# test_core
set(GMIO_TEST_CORE_SRC
        main_test_core.c
        test_core.c
        test_core_internal.c
        test_core_platform.c
        core_utils.c
        stream_buffer.c)
if(GMIO_BUILD_SHARED_LIBS)
    # Note_1: MSVC wants it because internal symbols are not exported (GCC
    #         exports all symbols)
    set(GMIO_TEST_CORE_SRC
            ${GMIO_TEST_CORE_SRC}
            ../src/gmio_core/internal/string_parse.c)
endif()

add_executable(test_core EXCLUDE_FROM_ALL ${GMIO_TEST_CORE_SRC})
target_link_libraries(test_core gmio)

# test_stl
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/models

     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
set(GMIO_TEST_STL_SRC
        main_test_stl.c
        test_stl_io.c
        test_stl_internal.c
        core_utils.c
        stl_utils.c)
if(GMIO_BUILD_SHARED_LIBS)
    # See Note_1
    set(GMIO_TEST_STL_SRC
            ${GMIO_TEST_STL_SRC}
            ../src/gmio_stl/internal/stl_rw_common.c)
endif()

add_executable(test_stl EXCLUDE_FROM_ALL ${GMIO_TEST_STL_SRC})
target_link_libraries(test_stl gmio)

# fake_support
if(GMIO_BUILD_TESTS_FAKE_SUPPORT)
    add_subdirectory(fake_support)
    add_dependencies(check fake_support)
endif()

# For some targets, GCC requires -lm
if(CMAKE_C_COMPILER_IS_GCC_COMPATIBLE)
    target_link_libraries(test_core m)
    target_link_libraries(test_stl m)
endif()

# Declare tests
add_test(test_core test_core)
add_test(test_stl test_stl)

add_dependencies(check test_core test_stl)
