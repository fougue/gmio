include(../../qmake.build/config.pri)
include(_local_config.pri)
include(occ.pri)

TEMPLATE = app
TARGET = bench_occ$$TARGET_SUFFIX

QT *= core

HEADERS +=

SOURCES += main.cpp

LIBS *= -lTKSTL -lTKernel

INCLUDEPATH += ../../src
LIBS += -L../../bin -lfougstl$$TARGET_SUFFIX
