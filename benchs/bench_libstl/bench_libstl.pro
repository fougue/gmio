include(../../qmake/config.pri)

CONFIG -= build_all

*-g++*:QMAKE_CFLAGS += -ansi -pedantic-errors

TEMPLATE = app
TARGET = bench_libstl$$TARGET_SUFFIX

INCLUDEPATH += $$GMIO_ROOT/include

HEADERS += ../commons/bench_tools.h

SOURCES += \
           ../commons/bench_tools.c \
           main.c

DEFINES += GMIO_USE_STDINT_H

LIBS += -L$$GMIO_ROOT/lib -lgmio$$TARGET_SUFFIX
QMAKE_RPATHDIR += $$GMIO_ROOT/lib
