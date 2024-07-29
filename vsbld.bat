@echo off
SET PROGNAME=imgexp
SET PLIBS="kernel32.lib advapi32.lib delayimp.lib user32.lib gdi32.lib comctl32.lib ole32.lib uuid.lib windowscodecs.lib"
SET COMPOPT="/DUNICODE /D_UNICODE /DDEBUG"
mkdir output 2>NUL:
cl "%COMPOPT%" /std:c17 /W3 /O2 /c limg2.c /Fo"output\limg2.obj"
cl "%COMPOPT%" /std:c17 /W3 /O2 /c "%PROGNAME%.c" /Fo"output\%PROGNAME%.obj"
rc /Fo"output\%PROGNAME%.res" "%PROGNAME%.rc"
link "output\%PROGNAME%.obj" "output\%PROGNAME%.res" output\limg2.obj "%PLIBS%"
