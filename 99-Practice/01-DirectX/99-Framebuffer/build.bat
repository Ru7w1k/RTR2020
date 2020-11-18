del main.res main.obj main.exe

cl.exe /c /EHsc main.cpp

rc.exe main.rc

link.exe main.obj main.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /NODEFAULTLIB:msvcrt.lib

main.exe
