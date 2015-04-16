@echo off
msbuild libjasper\libjasper.vcproj /p:Configuration=Release,Platform=Win32 >>..\..\..\compileopencv.txt
msbuild libjpeg\libjpeg.vcproj /p:Configuration=Release,Platform=Win32 >>..\..\..\compileopencv.txt
msbuild libpng\libpng.vcproj /p:Configuration=Release,Platform=Win32 >>..\..\..\compileopencv.txt
msbuild libtiff\libtiff.vcproj /p:Configuration=Release,Platform=Win32 >>..\..\..\compileopencv.txt
msbuild zlib\zlib.vcproj /p:Configuration=Release,Platform=Win32 >>..\..\..\compileopencv.txt
