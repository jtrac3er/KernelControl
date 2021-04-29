
:: Bessere Organisation für Prebuildereignisse
:: Alles wird nun hierhin geschrieben


echo Beginne Prebuildereignisse...

set ASMPATH=.\Assembler
set OBJPATH=.\Objectfiles

:: Achtung: Die Variable der Datei darf nur ein Zeichen lang sein
:: Sonst gibt es einen Syntax Error von der CMD

:: Weitere Infos über die Modifiers für Dateinamen (%%~nf --> Namen ohne Pfad und extension) findet man hier: 
:: https://superuser.com/questions/489240/how-to-get-filename-only-without-path-in-windows-command-line


for %%f in ("%ASMPATH%\*.asm") do nasm -o %OBJPATH%\%%~nf.obj -f win64 %%f


echo Prebuildereignisse abgeschlossen