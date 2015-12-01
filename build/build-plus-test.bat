@echo off

call build.bat


rem ########## Testing libraries ##########
echo;
echo ==========================================
echo Testing libraries
echo;

echo Building stksockettest.sln...
%DEVENV% "..\test\stksockettest\stksockettest.sln" /rebuild Release 
echo Building stkdatatest.sln...
%DEVENV% "..\test\stkdatatest\stkdatatest.sln" /rebuild Release
echo Building stkthreadtest.sln...
%DEVENV% "..\test\stkthreadtest\stkthreadtest.sln" /rebuild Release
echo Building commonfunctest.sln...
%DEVENV% "..\test\commonfunctest\commonfunctest.sln" /rebuild Release

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


echo;
echo Testing libraries has done.
echo;
pause
