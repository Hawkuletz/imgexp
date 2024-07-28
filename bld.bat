@echo off
SET PROGNAME=imgexp
mkdir output 2>NUL:
rem call "c:\Program Files\PellesC\Bin\povars32.bat"
del "%PROGNAME%.exe"
echo compiling %PROGNAME%
pocc.exe -std:C11 -Tx86-coff -Ot -Ob1 -fp:precise -W1 -Gz -Ze -Go "%PROGNAME%.c" -Fo"output\%PROGNAME%.obj"
porc.exe "%PROGNAME%.rc" -Fo"output\%PROGNAME%.res"
echo linking
polink.exe -subsystem:windows -machine:x86 kernel32.lib user32.lib gdi32.lib comctl32.lib comdlg32.lib advapi32.lib delayimp.lib shell32.lib -out:"%PROGNAME%.exe" "output\%PROGNAME%.obj" "output\%PROGNAME%.res"
dir "%PROGNAME%.exe"
