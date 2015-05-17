@echo off

set CURRENTPATH=%cd%
set DEVENV="C:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\IDE\devenv.exe"
set WKHTMLTOPDF="C:\Program Files\wkhtmltopdf\bin\wkhtmltopdf.exe"
set PDFTK="C:\Program Files (x86)\PDFtk Server\bin\pdftk.exe"
set SEVENZIP="C:\Program Files\7-Zip\7z.exe"

echo;
echo =========================================
echo Build YaizuComLib
echo =========================================

echo;
echo This batch file requires softwares shown below.
echo (1) Microsoft Visual Studio 2008 Professional Edition
echo (2) wkhtmltopdf 0.12.0.3
echo (3) PDFtk Server 2.02
echo (4) 7-Zip 9.20

if not exist %DEVENV% exit
if not exist %WKHTMLTOPDF% exit
if not exist %PDFTK% exit
if not exist %SEVENZIP% exit

echo;
echo Building process for YaizuComLib has started.
echo;


rem ########## Deleting previous output ##########
echo;
echo ==========================================
echo Deleting previous build folders...
echo;

if exist deployment rmdir /S /Q deployment
mkdir deployment


rem ########## Create PDF files ##########
echo;
echo ==========================================
echo Creating PDF files
echo;

pushd ..\doc\stksocket
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale frame_r.htm stksocket00.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section01.htm stksocket01.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section02.htm stksocket02.pdf
%PDFTK% *.pdf output stksocket.pdf
popd
copy ..\doc\stksocket\stksocket.pdf deployment
del ..\doc\stksocket\*.pdf

pushd ..\doc\stkthread
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale frame_r.htm stkthread00.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section01.htm stkthread01.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section02.htm stkthread02.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section03.htm stkthread03.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section05.htm stkthread05.pdf
%PDFTK% *.pdf output stkthread.pdf
popd
copy ..\doc\stkthread\stkthread.pdf deployment
del ..\doc\stkthread\*.pdf

pushd ..\doc\stkthreadgui
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale frame_r.htm stkthreadgui00.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section01.htm stkthreadgui01.pdf
%PDFTK% *.pdf output stkthreadgui.pdf
popd
copy ..\doc\stkthreadgui\stkthreadgui.pdf deployment
del ..\doc\stkthreadgui\*.pdf

pushd ..\doc\stkdata
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale frame_r.htm stkdata0000.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section01.htm stkdata0100.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section02.htm stkdata0200.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section03.htm stkdata0300.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04.htm stkdata0400.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-1.htm stkdata0401.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-2.htm stkdata0402.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-3.htm stkdata0403.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-4.htm stkdata0404.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-5.htm stkdata0405.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-6.htm stkdata0406.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-7.htm stkdata0407.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-8.htm stkdata0408.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-9.htm stkdata0409.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-10.htm stkdata0410.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-11.htm stkdata0411.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-12.htm stkdata0412.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-13.htm stkdata0413.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-14.htm stkdata0414.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section05.htm stkdata0500.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section06.htm stkdata0600.pdf
%PDFTK% *.pdf output stkdata.pdf
popd
copy ..\doc\stkdata\stkdata.pdf deployment
del ..\doc\stkdata\*.pdf


rem ########## Make libraries ##########
echo;
echo ==========================================
echo Making libraries
echo;

echo Building stksocket.sln...
%DEVENV% "..\src\stksocket\stksocket.sln" /rebuild Release 
copy "..\src\stksocket\stksocket.h" deployment
copy "..\src\stksocket\Release\stksocket.lib" deployment
%SEVENZIP% a ..\build\deployment\stksocket.zip ..\build\deployment\stksocket.lib
%SEVENZIP% a ..\build\deployment\stksocket.zip ..\build\deployment\stksocket.h
%SEVENZIP% a ..\build\deployment\stksocket.zip ..\build\deployment\stksocket.pdf

echo Building stkthread.sln...
%DEVENV% "..\src\stkthread\stkthread.sln" /rebuild Release 
copy "..\src\stkthread\stkthread.h" deployment
copy "..\src\stkthread\Release\stkthread.lib" deployment
%SEVENZIP% a ..\build\deployment\stkthread.zip ..\build\deployment\stkthread.lib
%SEVENZIP% a ..\build\deployment\stkthread.zip ..\build\deployment\stkthread.h
%SEVENZIP% a ..\build\deployment\stkthread.zip ..\build\deployment\stkthread.pdf

echo Building stkthreadgui.sln...
%DEVENV% "..\src\stkthreadgui\stkthreadgui.sln" /rebuild Release 
copy "..\src\stkthreadgui\stkthreadgui.h" deployment
copy "..\src\stkthreadgui\Release\stkthreadgui.lib" deployment
%SEVENZIP% a ..\build\deployment\stkthreadgui.zip ..\build\deployment\stkthreadgui.lib
%SEVENZIP% a ..\build\deployment\stkthreadgui.zip ..\build\deployment\stkthreadgui.h
%SEVENZIP% a ..\build\deployment\stkthreadgui.zip ..\build\deployment\stkthreadgui.pdf

echo Building stkdata.sln...
%DEVENV% "..\src\stkdata\stkdata.sln" /rebuild Release 
copy "..\src\stkdata\stkdata.h" deployment
copy "..\src\stkdata\Release\stkdata.lib" deployment
%SEVENZIP% a ..\build\deployment\stkdata.zip ..\build\deployment\stkdata.lib
%SEVENZIP% a ..\build\deployment\stkdata.zip ..\build\deployment\stkdata.h
%SEVENZIP% a ..\build\deployment\stkdata.zip ..\build\deployment\stkdata.pdf


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
..\test\stksockettest\release\\stksockettest.exe
echo ---------------------------------------
..\test\stkdatatest\release\stkdatatest.exe
echo ---------------------------------------
..\test\stkthreadtest\release\stkthreadtest.exe
echo ---------------------------------------
..\test\commonfunctest\release\commonfunctest.exe


echo;
echo Building process for YaizuComLib has ended.
echo;

