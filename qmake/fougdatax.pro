isEmpty(PREFIX_DIR) {
  PREFIX_DIR = ..
}

include(config.pri)

message($$PREFIX_DIR)

TEMPLATE = lib
TARGET = fougdatax$$TARGET_SUFFIX
DESTDIR = $$PREFIX_DIR/lib

QT =

DATAX += stl

dll:DEFINES += FOUG_LIB_DLL \
               FOUG_LIB_MAKE_DLL

#*g++*:QMAKE_CXXFLAGS_RELEASE -= -O2
#*g++*:QMAKE_CXXFLAGS_RELEASE += -O3

HEADERS += \
    ../src/endian.h \
    ../src/error.h \
    ../src/global.h \
    ../src/memory.h \
    ../src/stream.h \
    ../src/task_control.h \
    ../src/transfer.h \
    \
    ../src/internal/convert.h \
    ../src/internal/ascii_parse.h \
    ../src/internal/byte_swap.h \
    ../src/internal/byte_codec.h

SOURCES += \
    ../src/endian.c \
    ../src/error.c \
    ../src/stream.c \
    ../src/task_control.c \
    \
    ../src/internal/convert.c \
    ../src/internal/ascii_parse.c

*-g++*:QMAKE_CFLAGS += -ansi -pedantic-errors
*-msvc*:QMAKE_CFLAGS += -TC

global_inc.path  = $$PREFIX_DIR/include/datax
global_inc.files = ../src/*.h
INSTALLS += global_inc

# libSTL
contains(DATAX, stl) {
  dll:DEFINES += FOUG_DATAX_LIBSTL_DLL \
                 FOUG_DATAX_LIBSTL_MAKE_DLL
  #DEFINES += FOUG_STLB_READWRITE_ALIGNSAFE

  HEADERS += \
    ../src/libstl/stl_error.h \
    ../src/libstl/stl_format.h \
    ../src/libstl/stl_global.h \
    ../src/libstl/stl_triangle.h \
    ../src/libstl/stla_read.h \
    ../src/libstl/stla_write.h \
    ../src/libstl/stlb_read.h \
    ../src/libstl/stlb_write.h \
    ../src/libstl/stlb_triangle.h \
    \
    ../src/internal/libstl/stlb_byte_swap.h \
    ../src/internal/libstl/stlb_rw_common.h

  SOURCES += \
    ../src/libstl/stl_format.c \
    ../src/libstl/stla_read.c \
    ../src/libstl/stla_write.c \
    ../src/libstl/stlb_read.c \
    ../src/libstl/stlb_write.c \
    \
    ../src/internal/libstl/stlb_byte_swap.c \
    ../src/internal/libstl/stlb_rw_common.c

  libstl_inc.path  = $$PREFIX_DIR/include/datax/libstl
  libstl_inc.files = ../src/libstl/*.h
  INSTALLS += libstl_inc
} # contains(DATAX, stl)

# libSupport
HAS_SUPPORT = $$find(DATAX, support)
!isEmpty(HAS_SUPPORT) {
  HEADERS += ../src/support/support_global.h
  dll:DEFINES += FOUG_LIBSUPPORT_DLL \
                 FOUG_LIBSUPPORT_MAKE_DLL
  support_inc.path  = $$PREFIX_DIR/include/datax/support
  support_inc.files = ../src/support/support_global.h
  INSTALLS += support_inc
} # !isEmpty(HAS_SUPPORT)

# qt_support
contains(DATAX, qt_support) {
  HEADERS += ../src/support/qt_stream.h
  SOURCES += ../src/support/qt_stream.cpp

  QT = core

  qt_support_inc.path  = $$PREFIX_DIR/include/datax/support
  qt_support_inc.files = ../src/support/qt_stream.h
  INSTALLS += qt_support_inc
} # contains(DATAX, qt_support)

# occ_support
contains(DATAX, occ_support) {
  isEmpty(CASCADE_ROOT):error(Variable CASCADE_ROOT is not defined)

  INCLUDEPATH += $$CASCADE_ROOT/inc

  win32-*:DEFINES += WNT
  linux-*:DEFINES += LIN LININTEL OCC_CONVERT_SIGNALS
  *-64:DEFINES += _OCC64
  !win32:DEFINES += HAVE_CONFIG_H \
                    HAVE_FSTREAM \
                    HAVE_IOSTREAM \
                    HAVE_IOMANIP \
                    HAVE_LIMITS_H

  HEADERS += ../src/support/occ_libstl.h
  SOURCES += ../src/support/occ_libstl.cpp

  CASCADE_LIBPATH =
  unix:CASCADE_LIBPATH = $$CASCADE_ROOT/lib
  CONFIG(debug, debug|release) {
    win32-msvc2005:CASCADE_LIBPATH = $$CASCADE_ROOT/win32/vc8/libd
    win32-msvc2008:CASCADE_LIBPATH = $$CASCADE_ROOT/win32/vc9/lib
    win32-msvc2010:CASCADE_LIBPATH = $$CASCADE_ROOT/win32/vc10/libd
  } else {
    win32-msvc2005:CASCADE_LIBPATH = $$CASCADE_ROOT/win32/vc8/lib
    win32-msvc2008:CASCADE_LIBPATH = $$CASCADE_ROOT/win32/vc9/lib
    win32-msvc2010:CASCADE_LIBPATH = $$CASCADE_ROOT/win32/vc10/lib
  }

  LIBS += -L$$CASCADE_LIBPATH -lTKSTL -lTKernel -lTKMath
  QMAKE_RPATHDIR += $$CASCADE_LIBPATH

  occ_support_inc.path  = $$PREFIX_DIR/include/datax/support
  occ_support_inc.files = ../src/support/occ_libstl.h
  INSTALLS += occ_support_inc
} # contains(DATAX, occ_support)

#VER_MAJ = 0
#VER_MIN = 1
#VER_PAT = 0

OTHER_FILES += \
            ../doc/main_page.cpp \
            ../doc/Doxyfile \
            \
            ../src/config.h.cmake

