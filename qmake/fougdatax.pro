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

INCLUDEPATH += $$PWD/../src

#*g++*:QMAKE_CXXFLAGS_RELEASE -= -O2
#*g++*:QMAKE_CXXFLAGS_RELEASE += -O3

HEADERS += \
    config.h \
    ../src/datax_core/endian.h \
    ../src/datax_core/error.h \
    ../src/datax_core/global.h \
    ../src/datax_core/memory.h \
    ../src/datax_core/stream.h \
    ../src/datax_core/task_control.h \
    ../src/datax_core/transfer.h \
    \
    ../src/datax_core/internal/convert.h \
    ../src/datax_core/internal/ascii_parse.h \
    ../src/datax_core/internal/byte_swap.h \
    ../src/datax_core/internal/byte_codec.h

SOURCES += \
    ../src/datax_core/endian.c \
    ../src/datax_core/error.c \
    ../src/datax_core/stream.c \
    ../src/datax_core/task_control.c \
    \
    ../src/datax_core/internal/convert.c \
    ../src/datax_core/internal/ascii_parse.c

*-g++*:QMAKE_CFLAGS += -ansi -pedantic-errors
*-msvc*:QMAKE_CFLAGS += -TC

global_inc.path  = $$PREFIX_DIR/include/datax
global_inc.files = config.h ../src/*.h
INSTALLS += global_inc

# libSTL
contains(DATAX, stl) {
  dll:DEFINES += FOUG_DATAX_LIBSTL_DLL \
                 FOUG_DATAX_LIBSTL_MAKE_DLL
  #DEFINES += FOUG_STLB_READWRITE_ALIGNSAFE

  HEADERS += \
    ../src/datax_stl/stl_error.h \
    ../src/datax_stl/stl_format.h \
    ../src/datax_stl/stl_geom.h \
    ../src/datax_stl/stl_geom_creator.h \
    ../src/datax_stl/stl_global.h \
    ../src/datax_stl/stl_io.h \
    ../src/datax_stl/stl_triangle.h \
    \
    ../src/datax_stl/internal/stlb_byte_swap.h \
    ../src/datax_stl/internal/stl_rw_common.h

  SOURCES += \
    ../src/datax_stl/stl_format.c \
    ../src/datax_stl/stla_read.c \
    ../src/datax_stl/stla_write.c \
    ../src/datax_stl/stlb_read.c \
    ../src/datax_stl/stlb_write.c \
    \
    ../src/datax_stl/internal/stlb_byte_swap.c \
    ../src/datax_stl/internal/stl_rw_common.c

  libstl_inc.path  = $$PREFIX_DIR/include/datax_stl
  libstl_inc.files = ../src/datax_stl/*.h
  INSTALLS += libstl_inc
} # contains(DATAX, stl)

# libSupport
HAS_SUPPORT = $$find(DATAX, support)
!isEmpty(HAS_SUPPORT) {
  HEADERS += ../src/support/support_global.h
  dll:DEFINES += FOUG_LIBSUPPORT_DLL \
                 FOUG_LIBSUPPORT_MAKE_DLL
  support_inc.path  = $$PREFIX_DIR/include/datax/support
  support_inc.files = ../src/datax_support/support_global.h
  INSTALLS += support_inc
} # !isEmpty(HAS_SUPPORT)

# qt_support
contains(DATAX, qt_support) {
  qt_support_inc.path  = $$PREFIX_DIR/include/datax_support
  qt_support_inc.files = ../src/datax_support/qt_stream.h
  INSTALLS += qt_support_inc
} # contains(DATAX, qt_support)

# occ_support
contains(DATAX, occ_support) {
  occ_support_inc.path  = $$PREFIX_DIR/include/datax_support
  occ_support_inc.files = ../src/datax_support/occ_libstl.h
  INSTALLS += occ_support_inc
} # contains(DATAX, occ_support)

#VER_MAJ = 0
#VER_MIN = 1
#VER_PAT = 0

OTHER_FILES += \
            ../doc/main_page.cpp \
            ../doc/Doxyfile \
            \
            ../src/datax_core/config.h.cmake
