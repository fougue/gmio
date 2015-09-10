#!/bin/bash

cmake --version

# Install cpp-coveralls only if OS==linux and CC==gcc
if [ "${TRAVIS_COVERALLS}" = "ON" ]      \
    && [ "${TRAVIS_OS_NAME}" = "linux" ] \
    && [ "${CC}" = "gcc" ];
then
    export PYTHONUSERBASE=`pwd`/pip;
    pip install --user cpp-coveralls;
else
    export TRAVIS_COVERALLS=OFF;
fi
echo TRAVIS_COVERALLS=$TRAVIS_COVERALLS

# CMake
mkdir build && cd build
cmake .. -G "Unix Makefiles"       \
         -DCMAKE_DEBUG_POSTFIX=_d  \
         -DCMAKE_BUILD_TYPE=$TRAVIS_BUILD_TYPE \
         -DCMAKE_INSTALL_PREFIX=../install     \
         -DGMIO_BUILD_SHARED_LIBS=$TRAVIS_SHARED_LIBS  \
         -DGMIO_BUILD_STRICT_C90=$TRAVIS_STRICT_C90    \
         -DGMIO_BUILD_TESTS_FAKE_SUPPORT=ON            \
         -DGMIO_BUILD_TESTS_COVERAGE=$TRAVIS_COVERALLS

# Make
make -j4
make install

# Run unit tests
if [ "${TRAVIS_MAKE_CHECK}" = "ON" ]; then
    make check;
fi

# Coveralls
if [ "${TRAVIS_COVERALLS}" = "ON" ]; then
    $PYTHONUSERBASE/bin/coveralls   \
        --exclude benchmarks        \
        --exclude build/CMakeFiles  \
        --exclude install           \
        --exclude tests             \
        --gcov-options '\-lp'       \
        --root .. --build-root .    ;
fi
