del *.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale frame_r.htm stkthreadgui00.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section01.htm stkthreadgui01.pdf
"..\..\build\pdftk" *.pdf output stkthreadgui.pdf
