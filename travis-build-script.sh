#!/bin/bash

# Make the script fails on any command error
set -e

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

# CMake options as environment variables
if [ -n "${TRAVIS_GCC_VERSION}" ]; then
    export CC=gcc-$TRAVIS_GCC_VERSION;
    export CXX=g++-$TRAVIS_GCC_VERSION;
fi

if [ -z "${TRAVIS_FLOAT2STR_LIB}" ]; then
    export TRAVIS_FLOAT2STR_LIB=std;
fi

# Run CMake
mkdir build && cd build
cmake --version

cmake .. -G "Unix Makefiles"       \
         -DCMAKE_DEBUG_POSTFIX=_d  \
         -DCMAKE_BUILD_TYPE=$TRAVIS_BUILD_TYPE \
         -DCMAKE_INSTALL_PREFIX=../install     \
         -DGMIO_BUILD_EXAMPLES=ON                      \
         -DGMIO_BUILD_BENCHMARKS=ON                    \
         -DGMIO_BUILD_BENCHMARK_ASSIMP=OFF             \
         -DGMIO_BUILD_BENCHMARK_OPENCASCADE=OFF        \
         -DGMIO_BUILD_TESTS_FAKE_SUPPORT=ON            \
         -DGMIO_BUILD_TESTS_COVERAGE=$TRAVIS_COVERALLS \
         -DGMIO_FLOAT2STR_LIB=$TRAVIS_FLOAT2STR_LIB

# Make
make -j4 VERBOSE=1
make install
make check  # Unit tests

# Coveralls
if [ "${TRAVIS_COVERALLS}" = "ON" ]; then
    $PYTHONUSERBASE/bin/coveralls   \
        --exclude benchmarks        \
        --exclude build/CMakeFiles  \
        --exclude examples          \
        --exclude install           \
        --exclude tests             \
        --gcov-options '\-lp'       \
        --root .. --build-root .    ;
fi
