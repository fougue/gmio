isEmpty(PREFIX_DIR) {
  PREFIX_DIR = ..
}

include(config.pri)

message($$PREFIX_DIR)

TEMPLATE = lib
TARGET = fougdatax-c$$TARGET_SUFFIX
DESTDIR = $$PREFIX_DIR/lib

QT =

DATAX *= stl qt4_support occ_support

dll:DEFINES *= FOUG_LIB_DLL \
               FOUG_LIB_MAKE_DLL

#*g++*:QMAKE_CXXFLAGS_RELEASE -= -O2
#*g++*:QMAKE_CXXFLAGS_RELEASE += -O3

INCLUDEPATH += ../src

HEADERS += \
    ../src/c/endian.h \
    ../src/c/error.h \
    ../src/c/global.h \
    ../src/c/memory.h \
    ../src/c/stream.h \
    ../src/c/task_control.h \
    ../src/c/transfer.h \

SOURCES += \
    ../src/c/endian.c \
    ../src/c/error.c \
    ../src/c/stream.c \
    ../src/c/task_control.c

*-g++*:QMAKE_CFLAGS += -ansi -pedantic-errors
*-msvc*:QMAKE_CFLAGS += -TC

global_inc.path  = $$PREFIX_DIR/include/datax
global_inc.files = ../src/*.h
c_global_inc.path  = $$PREFIX_DIR/include/datax/c
c_global_inc.files = ../src/c/*.h
INSTALLS += global_inc  c_global_inc

# libSTL
contains(DATAX, stl) {
  dll:DEFINES *= FOUG_DATAX_LIBSTL_DLL \
                 FOUG_DATAX_LIBSTL_MAKE_DLL

  HEADERS += \
    ../src/c/libstl/stl_global.h \
    ../src/c/libstl/stla_read.h \
    ../src/c/libstl/stla_write.h \
    ../src/c/libstl/stlb_read.h \
    ../src/c/libstl/stlb_write.h \
    ../src/c/libstl/stl_triangle.h \
    ../src/c/libstl/stlb_triangle.h

  SOURCES += \
    ../src/c/libstl/stla_read.c \
    ../src/c/libstl/stla_write.c \
    ../src/c/libstl/stlb_read.c \
    ../src/c/libstl/stlb_write.c

  c_libstl_inc.path  = $$PREFIX_DIR/include/datax/c/libstl
  c_libstl_inc.files = ../src/c/libstl/*.h
  INSTALLS += c_libstl_inc
} # contains(DATAX, stl)

# libSupport
HAS_SUPPORT = $$find(DATAX, support)
!isEmpty(HAS_SUPPORT) {
  dll:DEFINES *= FOUG_LIBSUPPORT_DLL \
                 FOUG_LIBSUPPORT_MAKE_DLL
  support_inc.path  = $$PREFIX_DIR/include/datax/c/support
  support_inc.files = ../src/c/support/support_global.h
  INSTALLS += support_inc
} # !isEmpty(HAS_SUPPORT)

# qt4_support
contains(DATAX, qt4_support) {
  HEADERS += ../src/c/support/qt4_stream.h
  SOURCES += ../src/c/support/qt4_stream.cpp

  QT = core

  qt4_support_inc.path  = $$PREFIX_DIR/include/datax/c/support
  qt4_support_inc.files = ../src/c/support/qt4_stream.h
  INSTALLS += qt4_support_inc
} # contains(DATAX, qt4_support)

# occ_support
contains(DATAX, occ_support) {
  isEmpty(CASCADE_ROOT):error(Variable CASCADE_ROOT is not defined)

  INCLUDEPATH += $$CASCADE_ROOT/inc

  !win32:DEFINES *= HAVE_CONFIG_H \
                    HAVE_FSTREAM  \
                    HAVE_IOSTREAM \
                    HAVE_LIMITS_H

  HEADERS += ../src/c/support/occ_libstl.h
  SOURCES += ../src/c/support/occ_libstl.cpp

  occ_support_inc.path  = $$PREFIX_DIR/include/datax/c/support
  occ_support_inc.files = ../src/c/support/occ_libstl.h
  INSTALLS += occ_support_inc
} # contains(DATAX, occ_support)

#VER_MAJ = 0
#VER_MIN = 1
#VER_PAT = 0
