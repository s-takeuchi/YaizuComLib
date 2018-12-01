@echo off

call build.bat


rem ########## Testing libraries ##########
echo;
echo ==========================================
echo Testing libraries
echo;

echo Building stksockettest.sln...
%MSBUILD% "..\test\stksockettest\stksockettest.sln" /t:clean;build /p:Configuration=Release
echo Building stkdatatest.sln...
%MSBUILD% "..\test\stkdatatest\stkdatatest.sln" /t:clean;build /p:Configuration=Release
echo Building stkthreadtest.sln...
%MSBUILD% "..\test\stkthreadtest\stkthreadtest.sln" /t:clean;build /p:Configuration=Release
echo Building commonfunctest.sln...
%MSBUILD% "..\test\commonfunctest\commonfunctest.sln" /t:clean;build /p:Configuration=Release
echo Building stkwebapptest.sln...
%MSBUILD% "..\test\stkwebapptest\stkwebapptest.sln" /t:clean;build /p:Configuration=Release

echo ---------------------------------------
echo stksockettest.exe --- stksockettest.txt
..\test\stksockettest\release\\stksockettest.exe > stksockettest.txt
echo ---------------------------------------
echo stkdatatest.exe --- stkdatatest.txt
..\test\stkdatatest\release\stkdatatest.exe > stkdatatest.txt
echo ---------------------------------------
echo stkthreadtest.exe --- stkthreadtest.txt
..\test\stkthreadtest\release\stkthreadtest.exe > stkthreadtest.txt
echo ---------------------------------------
echo commonfunctest.exe --- commonfunctest.txt
..\test\commonfunctest\release\commonfunctest.exe > commonfunctest.txt
echo ---------------------------------------
echo stkwebapptest.exe --- stkwebapptest.txt
..\test\stkwebapptest\release\stkwebapptest.exe > stkwebapptest.txt


echo;
echo Testing libraries has done.
echo;
pause
