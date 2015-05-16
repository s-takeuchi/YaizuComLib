del *.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale frame_r.htm stkdata0000.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section01.htm stkdata0100.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section02.htm stkdata0200.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section03.htm stkdata0300.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04.htm stkdata0400.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-1.htm stkdata0401.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-2.htm stkdata0402.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-3.htm stkdata0403.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-4.htm stkdata0404.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-5.htm stkdata0405.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-6.htm stkdata0406.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-7.htm stkdata0407.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-8.htm stkdata0408.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-9.htm stkdata0409.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-10.htm stkdata0410.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-11.htm stkdata0411.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-12.htm stkdata0412.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-13.htm stkdata0413.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section04-14.htm stkdata0414.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section05.htm stkdata0500.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section06.htm stkdata0600.pdf
"..\..\build\pdftk" *.pdf output stkdata.pdf
