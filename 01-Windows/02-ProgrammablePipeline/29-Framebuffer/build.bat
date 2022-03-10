@REM cls

rc.exe OGLTemplate.rc

cl.exe /c /EHsc /MD /I"C:\glew\include" /I"C:\SOIL\inc" OGLTemplate.cpp 

link.exe /LIBPATH:"C:\glew\lib\Release\x64" /LIBPATH:"C:\SOIL\lib\x64" OGLTemplate.obj OGLTemplate.res user32.lib kernel32.lib gdi32.lib

OGLTemplate.exe
