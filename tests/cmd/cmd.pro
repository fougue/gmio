include(../../qmake.build/config.pri)

TEMPLATE = app
TARGET = cmd$$TARGET_SUFFIX

QT *= core

HEADERS +=

SOURCES += main.cpp

INCLUDEPATH += ../../src
LIBS += -L../../bin -lfougstl$$TARGET_SUFFIX
