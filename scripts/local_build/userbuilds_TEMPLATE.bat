rem -- Visual C++ 2013 x64
set %PATH%=%PATH_ORIGINAL%
set CURRENT_BUILD=msvc2013-x64
call %GMIO_LOCAL_CI_ROOT%\create_build_dirs.bat %CURRENT_BUILD%
call %GMIO_LOCAL_CI_ROOT%\build_msvc_target.bat debug   "%PATH_MSVC2013%" amd64
call %GMIO_LOCAL_CI_ROOT%\build_msvc_target.bat release "%PATH_MSVC2013%" amd64

rem -- Visual C++ 2013 x86
set %PATH%=%PATH_ORIGINAL%
set CURRENT_BUILD=msvc2013-x86
call %GMIO_LOCAL_CI_ROOT%\create_build_dirs.bat %CURRENT_BUILD%
set ROOTDIR_OPENCASCADE=
call %GMIO_LOCAL_CI_ROOT%\build_msvc_target.bat debug   "%PATH_MSVC2013%" x86
call %GMIO_LOCAL_CI_ROOT%\build_msvc_target.bat release "%PATH_MSVC2013%" x86

rem -- MinGW tdm64-gcc-5.1.0
set %PATH%=%ORIGINAL_PATH%
set CURRENT_BUILD=tdm64-gcc-5.1.0
call C:\dev\tools\tdm64-gcc-5.1.0\mingwvars.bat
call %GMIO_LOCAL_CI_ROOT%\create_build_dirs.bat %CURRENT_BUILD%
call %GMIO_LOCAL_CI_ROOT%\build_mingw_target.bat debug
call %GMIO_LOCAL_CI_ROOT%\build_mingw_target.bat release
