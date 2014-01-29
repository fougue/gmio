include(../../qmake.build/config.pri)

TEMPLATE = app
TARGET = bench_assimp$$TARGET_SUFFIX

INCLUDEPATH += $$ASSIMP_ROOT/include

HEADERS += ../commons/bench_tools.h

SOURCES += \
           ../commons/bench_tools.c \
           main.cpp

LIBS *= -L$$ASSIMP_ROOT/lib -lassimp
QMAKE_RPATHDIR *= $$ASSIMP_ROOT/lib
