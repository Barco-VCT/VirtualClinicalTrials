@echo off
set module=%1
IF NOT EXIST .\%module% (
echo please create the folder
echo help:
echo create the Cmake that describe the source code of a module
echo once the module is created, copy the name of the folder and give it in argument to this script
echo If your module contains more than 1 .cpp and 1 .h please complete the cmake
echo location of the Cmake: .\yourmodulename\cpp
goto end
)
cd .\%module%

cd .\cpp

if errorlevel 1 (
echo creation of the cpp folder
echo remember add it to the svn! and move %cd%\%module%.cpp to %cd%\cpp\
mkdir cpp
REM XCOPY /Y %module%.cpp .\cpp
cd cpp
)
echo SET(SRC_%module% >BuildFull.cmake
echo ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/%module%/cpp/%module%.cpp >>BuildFull.cmake
echo ) >>BuildFull.cmake
echo SET(HEADER_%module% >>BuildFull.cmake
echo ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/%module%/%module%.h >>BuildFull.cmake
echo ) >>BuildFull.cmake
echo SET(FILES_MODULES_%module% >>BuildFull.cmake
echo ${SRC_%module%} >>BuildFull.cmake
echo ${HEADER_%module%} >>BuildFull.cmake
echo ) >>BuildFull.cmake
cd ..\..


:end