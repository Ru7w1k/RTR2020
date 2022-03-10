cls

del OGLTemplate.obj
del OGLTemplate.exe

rc.exe OGLTemplate.rc

cl.exe /c /EHsc /I"C:\glew\include" OGLTemplate.cpp 

link.exe /LIBPATH:"C:\glew\lib\Release\win32" OGLTemplate.obj OGLTemplate.res user32.lib kernel32.lib gdi32.lib

OGLTemplate.exe
