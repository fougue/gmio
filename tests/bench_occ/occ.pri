use_oce {
  INCLUDEPATH += $$CASCADE_ROOT/include/oce
  !isEmpty(OCE_VERSION) {
    LIBS += -L$$CASCADE_ROOT/lib/$$OCE_VERSION
  }
}
else {
  INCLUDEPATH += $$CASCADE_ROOT/inc
}

linux-*:DEFINES *= HAVE_CONFIG_H \
                   HAVE_FSTREAM \
                   HAVE_IOSTREAM \
                   HAVE_LIMITS_H

win32-*:DEFINES *= WNT
linux-*:DEFINES *= LIN LININTEL OCC_CONVERT_SIGNALS
*-64:DEFINES *= _OCC64

linux-*:LIBS += -L$$CASCADE_ROOT/lib
win32-*:LIBS += -L$$CASCADE_ROOT/win32/lib

# There is a weird bug with qmake on windows : it fails to correctly link with
# TKSTEP209 due to the name of library mixing characters and digits.
# Or maybe nmake is the problem ?
# Note : you have to rename TKSTEP209 to TKSTEP_tzn in $CASROOT/win32/lib
win32-msvc* {
  OCC_TKSTEP = TKSTEP_tzn
}
else {
  OCC_TKSTEP = TKSTEP209
}
