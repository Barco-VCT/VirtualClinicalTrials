cls
@echo off

echo ##################################################################
echo ## Simulation
echo ## VCT: Virtual Clinical Trials
echo ##################################################################
echo ## Windows build script
echo ##################################################################
echo.


rem Command-line arguments
if "%1" == "opensrc" goto build
goto help

rem help information
:help
echo Wrong argument 
goto end

:build
rem building boost


echo Building boost ...



cd .\vct\src\Dependencies\boost

call bootstrap.bat
if errorlevel 1 (
echo bootstrap BUILD ERROR ( level 2
goto end
)
bjam --build-type=complete link=static runtime-link=shared toolset=msvc-9.0 --without-graph --without-iostreams --without-math --without-program_options --without-python --without-serialization --without-signals --without-system --without-test --without-wave
if errorlevel 1 (
echo bjam BUILD ERROR ( level 2
goto end
)
cd ..

building opencv
echo building opencv
cd OpenCV
call build.bat highgui cv cvaux cxcore
if errorlevel 1 (
echo OPENCV BUILD ERROR ( level 2
goto end
)
cd ..\..\..\..

rem Let's compile
set COMPILATION_TYPE=%1


echo Create build directory
if not exist win32 mkdir win32

rem Dir to build directory; run cmake; open project files and build
echo Dir to build directory
cd win32
echo Run cmake
cmake -G "Visual Studio 9 2008" -D COMPILATION_TYPE=%COMPILATION_TYPE% ..\vct\cmake
if errorlevel 1 (
echo cmake BUILD ERROR ( level 2
goto end
)
cd ..
echo.
echo Done, manual work to do:
echo.
echo                        Open project files
echo                        Build %CMAKE_BUILD_TYPE% configuration
goto end

:end
echo.
echo ##################################################################
echo.

