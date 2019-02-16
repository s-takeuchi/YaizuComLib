@echo off

if defined APPVEYOR (
  set MSBUILD="msbuild.exe"
)

if not defined APPVEYOR (
  set MSBUILD="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\msbuild.exe"
)

rem ########## Testing libraries ##########
echo;
echo ==========================================
echo Testing libraries
echo;

echo Building stksockettest.sln...
%MSBUILD% "..\test\stksockettest\stksockettest.sln" /t:clean;build /p:Configuration=Release
IF %ERRORLEVEL% NEQ 0 goto testexit
echo Building stkdatatest.sln...
%MSBUILD% "..\test\stkdatatest\stkdatatest.sln" /t:clean;build /p:Configuration=Release
IF %ERRORLEVEL% NEQ 0 goto testexit
echo Building stkthreadtest.sln...
%MSBUILD% "..\test\stkthreadtest\stkthreadtest.sln" /t:clean;build /p:Configuration=Release
IF %ERRORLEVEL% NEQ 0 goto testexit
echo Building commonfunctest.sln...
%MSBUILD% "..\test\commonfunctest\commonfunctest.sln" /t:clean;build /p:Configuration=Release
echo Building stkwebapptest.sln...
%MSBUILD% "..\test\stkwebapptest\stkwebapptest.sln" /t:clean;build /p:Configuration=Release
IF %ERRORLEVEL% NEQ 0 goto testexit

echo ---------------------------------------
echo stksockettest.exe
..\test\stksockettest\release\\stksockettest.exe
IF %ERRORLEVEL% NEQ 0 goto testexit
echo ---------------------------------------
echo stkdatatest.exe
..\test\stkdatatest\release\stkdatatest.exe
IF %ERRORLEVEL% NEQ 0 goto testexit
echo ---------------------------------------
echo stkthreadtest.exe
..\test\stkthreadtest\release\stkthreadtest.exe
IF %ERRORLEVEL% NEQ 0 goto testexit
echo ---------------------------------------
echo commonfunctest.exe
..\test\commonfunctest\release\commonfunctest.exe
IF %ERRORLEVEL% NEQ 0 goto testexit
echo ---------------------------------------
echo stkwebapptest.exe
..\test\stkwebapptest\release\stkwebapptest.exe
IF %ERRORLEVEL% NEQ 0 goto testexit

:testexit
echo;
echo Testing libraries has done.
echo;

if not defined APPVEYOR (
  pause
)
exit /b %ERRORLEVEL%
