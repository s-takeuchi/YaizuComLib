del *.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale frame_r.htm stkthread00.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section01.htm stkthread01.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section02.htm stkthread02.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section03.htm stkthread03.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section05.htm stkthread05.pdf
"..\..\build\pdftk" *.pdf output stkthread.pdf
