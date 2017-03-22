#!/bin/bash

if [ -z "${TRAVIS_TESTS_COVERAGE}" ]; then
    export TRAVIS_TESTS_COVERAGE=OFF
fi

if [ "$TRAVIS_TESTS_COVERAGE" = "ON" ]; then
    mkdir gcov; cd gcov
    list_src_path=(gmio_core gmio_core/internal
                   gmio_stl gmio_stl/internal
                   gmio_amf)
    basedir_output=../build/src/CMakeFiles/gmio_static.dir
    for src_path in ${list_src_path[*]}; do
        # Rename file.c.gcno -> file.gcno
        for file in $basedir_output/$src_path/*.c.gcno; do
            basename_file=`basename $file .c.gcno`
            mv $file $basedir_output/$src_path/$basename_file.gcno
        done
        # Rename file.c.gcda -> file.gcda
        for file in $basedir_output/$src_path/*.c.gcda; do
            basename_file=`basename $file .c.gcda`
            mv $file $basedir_output/$src_path/$basename_file.gcda
        done
        # Generate *.gcov files
        gcov ../src/$src_path/*.c -o $basedir_output/$src_path
    done
    bash <(curl -s https://codecov.io/bash) -X gcov -Xcoveragepy -s .
fi
