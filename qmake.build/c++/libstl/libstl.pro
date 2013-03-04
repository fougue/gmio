isEmpty(PREFIX_DIR) {
  PREFIX_DIR = ../../..
}

include(../../config.pri)

message($$PREFIX_DIR)

TEMPLATE = lib
TARGET = fougstl-c++$$TARGET_SUFFIX
DESTDIR = $$PREFIX_DIR/lib

DEFINES *= FOUG_USE_STDINT_H

dll {
  DEFINES *= FOUG_STL_DLL FOUG_STL_MAKE_DLL
}

#*g++*:QMAKE_CXXFLAGS_RELEASE -= -O2
#*g++*:QMAKE_CXXFLAGS_RELEASE += -O3

INCLUDEPATH += ../../../src

HEADERS += \
    ../../../src/c++/io_base.h \
    ../../../src/c++/foug_global.h \
    ../../../src/c++/abstract_task_progress.h \
    ../../../src/c++/abstract_stream.h \
    ../../../src/c++/streams/std_io_stream.h \
    ../../../src/c++/streams/qt4_stream.h \
    ../../../src/c++/libstl/abstract_geometry.h \
    ../../../src/c++/libstl/stlb.h \
    ../../../src/c++/libstl/stla.h \
    ../../../src/c++/libstl/stl_global.h

SOURCES += \
    ../../../src/c++/io_base.cpp \
    ../../../src/c++/abstract_task_progress.cpp \
#    ../../src/streams/std_io_stream.cpp \
    ../../../src/c++/streams/qt4_stream.cpp \
    ../../../src/c++/libstl/stlb.cpp \
    ../../../src/c++/libstl/stla.cpp

global_inc.path  = $$PREFIX_DIR/include/datax
global_inc.files = ../../../src/*.h
cpp_global_inc.path  = $$PREFIX_DIR/include/datax/c++
cpp_global_inc.files = ../../../src/c++/*.h
cpp_streams_inc.path  = $$PREFIX_DIR/include/datax/c++/streams
cpp_streams_inc.files = ../../../src/c++/streams/*.h
cpp_libstl_inc.path  = $$PREFIX_DIR/include/datax/c++/libstl
cpp_libstl_inc.files = ../../../src/c++/libstl/*.h
INSTALLS += global_inc  cpp_global_inc  cpp_streams_inc  cpp_libstl_inc

VER_MAJ = 0
VER_MIN = 1
VER_PAT = 0
