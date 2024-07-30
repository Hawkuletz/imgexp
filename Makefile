# compile with nmake from Visual Studio (Developer Command Prompt)
CC = CL
LINK = LINK
CFLAGS = /std:c17 /W3 /O2 /c
DEFINES = /DUNICODE /D_UNICODE /DDEBUG
PLIBS = kernel32.lib advapi32.lib delayimp.lib user32.lib gdi32.lib comctl32.lib ole32.lib uuid.lib windowscodecs.lib
OBJS = img2dc.obj imgexp.obj imgexp.res
PROGRAM = imgexp.exe

all: $(PROGRAM)

# any change to headers should trigger a full recompile
$(OBJS):*.h

.c.obj:
	$(CC) $(CFLAGS) $(DEFINES) $*.c

.rc.res:
	$(RC) $*.rc

imgexp.exe: $(OBJS)
	$(LINK) -out:imgexp.exe $(OBJS) $(PLIBS)

clean:
	del *.obj *.res *.exe
