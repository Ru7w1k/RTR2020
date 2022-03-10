cls

del RMCLog.txt 
del D3D11Template.obj 
del D3D11Template.res 
del D3D11Template.exe

rc.exe D3D11Template.rc

cl.exe /c /EHsc D3D11Template.cpp 

link.exe /NODEFAULTLIB:msvcrt.lib D3D11Template.obj D3D11Template.res user32.lib kernel32.lib gdi32.lib

D3D11Template.exe
