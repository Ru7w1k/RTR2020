del main.exe main.obj main.res

cl.exe /c /EHsc main.cpp

rc.exe main.rc

link.exe main.obj main.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

main.exe
