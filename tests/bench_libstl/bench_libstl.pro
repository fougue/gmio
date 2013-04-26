include(../../qmake.build/config.pri)

TEMPLATE = app
TARGET = bench_libstl$$TARGET_SUFFIX

INCLUDEPATH += $$FOUG_DATAX_ROOT/include

HEADERS += ../commons/bench_tools.h

SOURCES += \
           ../commons/bench_tools.c \
           main.c

DEFINES += FOUG_USE_STDINT_H

LIBS *= -L$$FOUG_DATAX_ROOT/lib -lfougdatax-c$$TARGET_SUFFIX
QMAKE_RPATHDIR *= $$FOUG_DATAX_ROOT/lib
