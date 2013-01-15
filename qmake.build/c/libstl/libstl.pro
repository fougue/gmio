isEmpty(PREFIX_DIR) {
  PREFIX_DIR = ../../..
}

include(../../config.pri)

message($$PREFIX_DIR)

TEMPLATE = lib
TARGET = fougstl-c$$TARGET_SUFFIX
DESTDIR = $$PREFIX_DIR/lib

dll {
  DEFINES *= FOUG_LIB_DLL  FOUG_LIB_MAKE_DLL
  DEFINES *= FOUG_DATAEX_LIBSTL_DLL  FOUG_DATAEX_LIBSTL_MAKE_DLL
}

#*g++*:QMAKE_CXXFLAGS_RELEASE -= -O2
#*g++*:QMAKE_CXXFLAGS_RELEASE += -O3

INCLUDEPATH += ../../../src

HEADERS += \
    ../../../src/c/endian.h \
    ../../../src/c/global.h \
    ../../../src/c/memory.h \
    ../../../src/c/stream.h \
    ../../../src/c/task_control.h \
    ../../../src/c/libstl/stl_global.h \
    ../../../src/c/libstl/stlb_read.h \
    ../../../src/c/libstl/stlb_write.h \
    ../../../src/c/libstl/stl_triangle.h

SOURCES += \
    ../../../src/c/endian.c \
    ../../../src/c/stream.c \
    ../../../src/c/task_control.c \
    ../../../src/c/libstl/stlb_read.c \
    ../../../src/c/libstl/stlb_write.c

*-g++*:QMAKE_CFLAGS += -ansi

global_inc.path  = $$PREFIX_DIR/include/dataex
global_inc.files = ../../../src/*.h
c_global_inc.path  = $$PREFIX_DIR/include/dataex/c
c_global_inc.files = ../../../src/c/*.h
c_streams_inc.path  = $$PREFIX_DIR/include/dataex/c/streams
c_streams_inc.files = ../../../src/c/streams/*.h
c_libstl_inc.path  = $$PREFIX_DIR/include/dataex/c/libstl
c_libstl_inc.files = ../../../src/c/libstl/*.h
INSTALLS += global_inc  c_global_inc  c_streams_inc  c_libstl_inc

VER_MAJ = 0
VER_MIN = 1
VER_PAT = 0
