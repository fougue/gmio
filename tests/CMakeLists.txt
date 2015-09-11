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

# test_internal
add_executable(
    test_internal
    EXCLUDE_FROM_ALL
        stream_buffer.c
        utils.c
        test_internal.c
        ../src/gmio_core/internal/string_parse.c)
#target_link_libraries(test_internal gmio)

# test_platform
add_executable(
    test_platform EXCLUDE_FROM_ALL test_platform.c)

# test_core
add_executable(
    test_core EXCLUDE_FROM_ALL test_core.c)
target_link_libraries(test_core gmio)

# test_stl
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/models
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_executable(
    test_stl
    EXCLUDE_FROM_ALL
        test_stl_io.c
        stl_utils.c
        utils.c)
target_link_libraries(test_stl gmio)

# fake_support
if(GMIO_BUILD_TESTS_FAKE_SUPPORT)
    add_subdirectory(fake_support)
    add_dependencies(check fake_support)
endif()

# For some targets, GCC and Clang requires -lm
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG)
    target_link_libraries(test_internal m)
    target_link_libraries(test_stl m)
endif()

# Declare tests
add_test(test_internal test_internal)
add_test(test_platform test_platform)
add_test(test_core test_core)
add_test(test_stl test_stl)

add_dependencies(
    check
        test_internal
        test_platform
        test_core
        test_stl)
