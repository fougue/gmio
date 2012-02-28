include(compiler_config.pri)
CONFIG(debug, debug|release) {
  TARGET_SUFFIX = .debug
} else {
  TARGET_SUFFIX = .release
}
CONFIG *= build_all debug_and_release
