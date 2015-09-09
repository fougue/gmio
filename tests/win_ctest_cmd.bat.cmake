@echo off

set PATH=@CMAKE_BINARY_DIR@\src;%PATH%
ctest -V
