@echo off
SET PROGNAME=imgexp
SET PLIBS="kernel32.lib advapi32.lib delayimp.lib user32.lib gdi32.lib comctl32.lib ole32.lib uuid.lib windowscodecs.lib"
SET COMPOPT="-DUNICODE -D_UNICODE -DDEBUG"
mkdir output 2>NUL:
rem call "c:\Program Files\PellesC\Bin\povars32.bat"
del "%PROGNAME%.exe"
echo compiling %PROGNAME%
pocc.exe -std:C11 -Tx86-coff -Ot -Ob1 -fp:precise -W1 -Gz -Ze -Go "%COMPOPT%" img2dc.c -Fo"output\img2dc.obj"
pocc.exe -std:C11 -Tx86-coff -Ot -Ob1 -fp:precise -W1 -Gz -Ze -Go "%COMPOPT%" "%PROGNAME%.c" -Fo"output\%PROGNAME%.obj"
porc.exe "%PROGNAME%.rc" -Fo"output\%PROGNAME%.res"
echo linking
polink.exe -subsystem:windows -machine:x86 "%PLIBS%" -out:"%PROGNAME%.exe" "output\%PROGNAME%.obj" "output\%PROGNAME%.res" "output\img2dc.obj"
dir "%PROGNAME%.exe"
