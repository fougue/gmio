include(../config.pri)

TEMPLATE = lib
TARGET = fougstl$$TARGET_SUFFIX
DESTDIR = ../../bin
CONFIG *= dll

dll {
  DEFINES *= FOUG_STL_DLL FOUG_STL_MAKE_DLL
}

#*g++*:QMAKE_CXXFLAGS_RELEASE -= -O2
#*g++*:QMAKE_CXXFLAGS_RELEASE += -O3

INCLUDEPATH += ../../src

HEADERS += \
    ../../src/io_base.h \
    ../../src/foug_global.h \
    ../../src/abstract_task_progress.h \
    ../../src/abstract_stream.h \
    ../../src/streams/std_io_stream.h \
    ../../src/streams/qt4_stream.h \
    ../../src/libstl/abstract_geometry.h \
    ../../src/libstl/stlb.h \
    ../../src/libstl/stla.h \
    ../../src/libstl/stl_global.h

SOURCES += \
    ../../src/io_base.cpp \
    ../../src/abstract_task_progress.cpp \
#    ../../src/streams/std_io_stream.cpp \
    ../../src/streams/qt4_stream.cpp \
    ../../src/libstl/stlb.cpp \
    ../../src/libstl/stla.cpp

VER_MAJ = 0
VER_MIN = 1
VER_PAT = 0
