@echo off

set CURRENTPATH=%cd%
set DEVENV="C:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\IDE\devenv.exe"
set WKHTMLTOPDF="C:\Program Files\wkhtmltopdf\bin\wkhtmltopdf.exe"
set PDFTK="C:\Program Files (x86)\PDFtk Server\bin\pdftk.exe"
set SEVENZIP="C:\Program Files\7-Zip\7z.exe"
set LCOUNTER="C:\Program Files (x86)\lcounter\lcounter.exe"

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
echo (5) Line Counter

if not exist %DEVENV% exit
if not exist %WKHTMLTOPDF% exit
if not exist %PDFTK% exit
if not exist %SEVENZIP% exit
if not exist %LCOUNTER% exit

echo;
echo Building process for YaizuComLib has started.
echo;


rem ########## Deleting previous output ##########
echo;
echo ==========================================
echo Deleting previous build folders...
echo;

if exist deployment rmdir /S /Q deployment
if exist deployment del deployment
mkdir deployment


rem ########## Create PDF files ##########
echo;
echo ==========================================
echo Creating PDF files
echo;

xcopy /y /q /s /i "..\doc\stksocket" deployment\stksocket
pushd deployment\stksocket
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale frame_r.htm stksocket00.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section01.htm stksocket01.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section02.htm stksocket02.pdf
%PDFTK% *.pdf output stksocket.pdf
copy stksocket.pdf ..
del *.pdf
popd

xcopy /y /q /s /i "..\doc\commonfunc" deployment\commonfunc
pushd deployment\commonfunc
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale frame_r.htm commonfunc00.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section01.htm commonfunc01.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section02.htm commonfunc02.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section03.htm commonfunc03.pdf
%PDFTK% *.pdf output commonfunc.pdf
copy commonfunc.pdf ..
del *.pdf
popd

xcopy /y /q /s /i "..\doc\stkthread" deployment\stkthread
pushd deployment\stkthread
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale frame_r.htm stkthread00.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section01.htm stkthread01.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section02.htm stkthread02.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section03.htm stkthread03.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section05.htm stkthread05.pdf
%PDFTK% *.pdf output stkthread.pdf
copy stkthread.pdf ..
del *.pdf
popd

xcopy /y /q /s /i "..\doc\stkthreadgui" deployment\stkthreadgui
pushd deployment\stkthreadgui
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale frame_r.htm stkthreadgui00.pdf
%WKHTMLTOPDF% --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section01.htm stkthreadgui01.pdf
%PDFTK% *.pdf output stkthreadgui.pdf
copy stkthreadgui.pdf ..
del *.pdf
popd

xcopy /y /q /s /i "..\doc\stkdata" deployment\stkdata
pushd deployment\stkdata
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
copy stkdata.pdf ..
del *.pdf
popd


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
del ..\build\deployment\stksocket.lib
del ..\build\deployment\stksocket.h
del ..\build\deployment\stksocket.pdf

echo Building commonfunc.sln...
%DEVENV% "..\src\commonfunc\commonfunc.sln" /rebuild Release 
copy "..\src\commonfunc\msgproc.h" deployment
copy "..\src\commonfunc\StkGeneric.h" deployment
copy "..\src\commonfunc\StkProperties.h" deployment
copy "..\src\commonfunc\Release\commonfunc.lib" deployment
%SEVENZIP% a ..\build\deployment\commonfunc.zip ..\build\deployment\commonfunc.lib
%SEVENZIP% a ..\build\deployment\commonfunc.zip ..\build\deployment\msgproc.h
%SEVENZIP% a ..\build\deployment\commonfunc.zip ..\build\deployment\StkGeneric.h
%SEVENZIP% a ..\build\deployment\commonfunc.zip ..\build\deployment\StkProperties.h
%SEVENZIP% a ..\build\deployment\commonfunc.zip ..\build\deployment\commonfunc.pdf
del ..\build\deployment\commonfunc.lib
del ..\build\deployment\msgproc.h
del ..\build\deployment\StkGeneric.h
del ..\build\deployment\StkProperties.h
del ..\build\deployment\commonfunc.pdf

echo Building stkthread.sln...
%DEVENV% "..\src\stkthread\stkthread.sln" /rebuild Release 
copy "..\src\stkthread\stkthread.h" deployment
copy "..\src\stkthread\Release\stkthread.lib" deployment
%SEVENZIP% a ..\build\deployment\stkthread.zip ..\build\deployment\stkthread.lib
%SEVENZIP% a ..\build\deployment\stkthread.zip ..\build\deployment\stkthread.h
%SEVENZIP% a ..\build\deployment\stkthread.zip ..\build\deployment\stkthread.pdf
del ..\build\deployment\stkthread.lib
del ..\build\deployment\stkthread.h
del ..\build\deployment\stkthread.pdf

echo Building stkthreadgui.sln...
%DEVENV% "..\src\stkthreadgui\stkthreadgui.sln" /rebuild Release 
copy "..\src\stkthreadgui\stkthreadgui.h" deployment
copy "..\src\stkthreadgui\Release\stkthreadgui.lib" deployment
%SEVENZIP% a ..\build\deployment\stkthreadgui.zip ..\build\deployment\stkthreadgui.lib
%SEVENZIP% a ..\build\deployment\stkthreadgui.zip ..\build\deployment\stkthreadgui.h
%SEVENZIP% a ..\build\deployment\stkthreadgui.zip ..\build\deployment\stkthreadgui.pdf
del ..\build\deployment\stkthreadgui.lib
del ..\build\deployment\stkthreadgui.h
del ..\build\deployment\stkthreadgui.pdf

echo Building stkdata.sln...
%DEVENV% "..\src\stkdata\stkdata.sln" /rebuild Release 
copy "..\src\stkdata\stkdata.h" deployment
copy "..\src\stkdata\stkdataapi.h" deployment
copy "..\src\stkdata\Release\stkdata.lib" deployment
%SEVENZIP% a ..\build\deployment\stkdata.zip ..\build\deployment\stkdata.lib
%SEVENZIP% a ..\build\deployment\stkdata.zip ..\build\deployment\stkdata.h
%SEVENZIP% a ..\build\deployment\stkdata.zip ..\build\deployment\stkdataapi.h
%SEVENZIP% a ..\build\deployment\stkdata.zip ..\build\deployment\stkdata.pdf
del ..\build\deployment\stkdata.lib
del ..\build\deployment\stkdata.h
del ..\build\deployment\stkdataapi.h
del ..\build\deployment\stkdata.pdf


echo;
%LCOUNTER% ..\src /subdir
echo;
echo Building process for YaizuComLib has ended.
echo;
pause
