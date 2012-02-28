CONFIG *= debug_and_release
CONFIG *= warn_on stl rtti exceptions

QT -= core gui

CONFIG(debug, debug|release) {
  DEFINES *= _DEBUG_CONFIG_
  CONFIG  *= console
}
else {
  CONFIG  -= console
}

CONFIG(warn_on) {
  *-g++*:QMAKE_CXXFLAGS *= -Wextra
}

win32-msvc20*:QMAKE_CXXFLAGS *= -wd4996 -wd4290 -wd4503
win32-msvc*:DEFINES *= NOMINMAX
# This explicitely defines INTXX_C() and UINTXX_C() macros in <stdint.h>
DEFINES *= __STDC_CONSTANT_MACROS
