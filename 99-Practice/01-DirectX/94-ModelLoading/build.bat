cls

cl.exe /c /EHsc /I"C:\Assimp\include" main.cpp

rc.exe main.rc

link.exe /LIBPATH:"C:\Assimp\lib\win32" main.obj main.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS /NODEFAULTLIB:msvcrt.lib

main.exe
