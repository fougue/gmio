include(../../qmake.build/config.pri)
include(occ.pri)

TEMPLATE = app
TARGET = bench_occ$$TARGET_SUFFIX

HEADERS += ../commons/bench_tools.h

SOURCES += \
           ../commons/bench_tools.c \
           main.cpp

LIBS *= -lTKSTL -lTKernel -lTKMath
QMAKE_RPATHDIR *= $$CASCADE_LIB_PATH
