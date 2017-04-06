if defined GMIO_BUILD_ROOT (
    cd %GMIO_BUILD_ROOT%
    rmdir /S /Q %1
    mkdir %1
    cd %1
    mkdir debug release install
)
