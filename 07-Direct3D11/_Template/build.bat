cls

rc.exe D3D11.rc

cl.exe /c /EHsc D3D11.cpp 

link.exe D3D11.obj D3D11.res user32.lib kernel32.lib gdi32.lib

D3D11.exe
