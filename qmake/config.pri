include(compiler_config.pri)
CONFIG(debug, debug|release) {
  TARGET_SUFFIX = _d
} else {
  TARGET_SUFFIX = 
}
CONFIG += build_all debug_and_release
