@echo off
set initpath=%windir%\Microsoft.NET\Framework\

IF "_%VS90COMNTOOLS%_"=="__" goto skip9
SET TOOLSET=Visual Studio 9 2008
REM FOR /F %%G IN ('DIR /b /O:N %initpath%v3*') DO (
REM dir /b %initpath%%%G\msbuild.exe> nul 2>&1 && set msbuild=%initpath%%%G\msbuild.exe
 REM )
 call "%VS90COMNTOOLS%"\vsvars32.bat
 goto :end
 :skip9
 echo you need VS9.0 to build the library
 :end



echo compile cxcore 's library
echo > compileopencv.txt 
msbuild cxcore\src\cxcore.vcproj /p:Configuration=Release,Platform=Win32>>compileopencv.txt
:Loop
if "%1"=="" goto Continue
echo compile %1 's library
if "_%1_"=="_highgui_" (
cd otherlibs\_graphics\src
call build.bat
cd ..\..
msbuild highgui\highgui.vcproj /p:Configuration=Release,Platform=Win32 >>..\compileopencv.txt
cd ..
shift
goto loop
)
msbuild %1\src\%1.vcproj /p:Configuration=Release,Platform=Win32 >>compileopencv.txt
shift
goto Loop
:Continue