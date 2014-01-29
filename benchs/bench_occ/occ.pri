# Return the input path re-written using the system-dependent separator
defineReplace(sysPath) {
  win*:result = $$replace(1, /, \\)
  else:result = $$1
  return($$result)
}

use_oce {
  CASCADE_INC_PATH = $$CASCADE_ROOT/include/oce
  !isEmpty(OCE_VERSION) {
    CASCADE_LIB_PATH += -$CASCADE_ROOT/lib/$$OCE_VERSION
  }
}
else {
  CASCADE_INC_PATH = $$CASCADE_ROOT/inc
}
INCLUDEPATH += $$CASCADE_INC_PATH

linux-*:DEFINES *= HAVE_CONFIG_H \
                   HAVE_FSTREAM \
                   HAVE_IOSTREAM \
                   HAVE_LIMITS_H

win32-*:DEFINES *= WNT
linux-*:DEFINES *= LIN LININTEL OCC_CONVERT_SIGNALS
*-64:DEFINES *= _OCC64

linux-*:CASCADE_LIB_PATH += $$CASCADE_ROOT/lib
CONFIG(debug, debug|release) {
  win32-msvc2005:CASCADE_LIB_PATH += $$CASCADE_ROOT/win32/vc8/libd
  win32-msvc2008:CASCADE_LIB_PATH += $$CASCADE_ROOT/win32/vc9/lib
  win32-msvc2010:CASCADE_LIB_PATH += $$CASCADE_ROOT/win32/vc10/libd
} else {
  win32-msvc2005:CASCADE_LIB_PATH += $$CASCADE_ROOT/win32/vc8/lib
  win32-msvc2008:CASCADE_LIB_PATH += $$CASCADE_ROOT/win32/vc9/lib
  win32-msvc2010:CASCADE_LIB_PATH += $$CASCADE_ROOT/win32/vc10/lib
}
LIBS += $$sysPath($$join(CASCADE_LIB_PATH, " -L", -L))

# There is a weird bug with qmake on windows : it fails to correctly link with TKSTEP209 due to the
# name of library mixing characters and digits.
#   Or maybe nmake is the problem ?
#   Note : you have to rename TKSTEP209 to TKSTEP_tzn in $$CASCADE_ROOT/win32/lib
win32-msvc* {
  OCC_TKSTEP = TKSTEP_tzn
}
else {
  OCC_TKSTEP = TKSTEP209
}
