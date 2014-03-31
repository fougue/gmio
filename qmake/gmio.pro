isEmpty(PREFIX_DIR) {
  PREFIX_DIR = ..
}

include(config.pri)

message($$PREFIX_DIR)

TEMPLATE = lib
TARGET = gmio$$TARGET_SUFFIX
DESTDIR = $$PREFIX_DIR/lib

QT =

GMIO += stl

dll:DEFINES += GMIO_LIB_DLL \
               GMIO_LIB_MAKE_DLL

INCLUDEPATH += $$PWD/../src

#*g++*:QMAKE_CXXFLAGS_RELEASE -= -O2
#*g++*:QMAKE_CXXFLAGS_RELEASE += -O3

HEADERS += \
    config.h \
    ../src/gmio_core/endian.h \
    ../src/gmio_core/error.h \
    ../src/gmio_core/global.h \
    ../src/gmio_core/memory.h \
    ../src/gmio_core/stream.h \
    ../src/gmio_core/task_control.h \
    ../src/gmio_core/transfer.h \
    \
    ../src/gmio_core/internal/convert.h \
    ../src/gmio_core/internal/ascii_parse.h \
    ../src/gmio_core/internal/byte_swap.h \
    ../src/gmio_core/internal/byte_codec.h \
    ../src/gmio_stl/stl_mesh.h \
    ../src/gmio_stl/stl_mesh_creator.h

SOURCES += \
    ../src/gmio_core/endian.c \
    ../src/gmio_core/error.c \
    ../src/gmio_core/stream.c \
    ../src/gmio_core/task_control.c \
    \
    ../src/gmio_core/internal/convert.c \
    ../src/gmio_core/internal/ascii_parse.c

*-g++*:QMAKE_CFLAGS += -ansi -pedantic-errors
*-msvc*:QMAKE_CFLAGS += -TC

global_inc.path  = $$PREFIX_DIR/include/gmio_core
global_inc.files = config.h ../src/*.h
INSTALLS += global_inc

# libSTL
contains(GMIO, stl) {
  dll:DEFINES += GMIO_LIBSTL_DLL \
                 GMIO_LIBSTL_MAKE_DLL

  HEADERS += \
    ../src/gmio_stl/stl_error.h \
    ../src/gmio_stl/stl_format.h \
    ../src/gmio_stl/stl_global.h \
    ../src/gmio_stl/stl_io.h \
    ../src/gmio_stl/stl_triangle.h \
    \
    ../src/gmio_stl/internal/stlb_byte_swap.h \
    ../src/gmio_stl/internal/stl_rw_common.h

  SOURCES += \
    ../src/gmio_stl/stl_format.c \
    ../src/gmio_stl/stla_read.c \
    ../src/gmio_stl/stla_write.c \
    ../src/gmio_stl/stlb_read.c \
    ../src/gmio_stl/stlb_write.c \
    \
    ../src/gmio_stl/internal/stlb_byte_swap.c \
    ../src/gmio_stl/internal/stl_rw_common.c

  libstl_inc.path  = $$PREFIX_DIR/include/gmio_stl
  libstl_inc.files = ../src/gmio_stl/*.h
  INSTALLS += libstl_inc
} # contains(GMIO, stl)

# libSupport
HAS_SUPPORT = $$find(GMIO, support)
!isEmpty(HAS_SUPPORT) {
  HEADERS += ../src/support/support_global.h
  dll:DEFINES += GMIO_LIBSUPPORT_DLL \
                 GMIO_LIBSUPPORT_MAKE_DLL
  support_inc.path  = $$PREFIX_DIR/include/gmio/support
  support_inc.files = ../src/gmio_support/support_global.h
  INSTALLS += support_inc
} # !isEmpty(HAS_SUPPORT)

# qt_support
contains(GMIO, qt_support) {
  qt_support_inc.path  = $$PREFIX_DIR/include/gmio_support
  qt_support_inc.files = ../src/gmio_support/qt_stream.h
  INSTALLS += qt_support_inc
} # contains(GMIO, qt_support)

# occ_support
contains(GMIO, occ_support) {
  occ_support_inc.path  = $$PREFIX_DIR/include/gmio_support
  occ_support_inc.files = ../src/gmio_support/occ_libstl.h
  INSTALLS += occ_support_inc
} # contains(GMIO, occ_support)

#VER_MAJ = 0
#VER_MIN = 1
#VER_PAT = 0

OTHER_FILES += \
            ../doc/main_page.cpp \
            ../doc/Doxyfile \
            \
            ../src/gmio_core/config.h.cmake
