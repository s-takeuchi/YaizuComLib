del *.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale frame_r.htm stksocket00.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section01.htm stksocket01.pdf
"..\..\build\wkhtmltopdf.exe" --zoom 1.3 --disable-internal-links --disable-external-links --grayscale section02.htm stksocket02.pdf
"..\..\build\pdftk" *.pdf output stksocket.pdf
