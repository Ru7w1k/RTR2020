cls

del *.obj *.exe

rc.exe OGLTemplate.rc

cl.exe /c /EHsc /I"C:\glew\include" /I"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.3\include" OGLTemplate.cpp 

link.exe /LIBPATH:"C:\glew\lib\Release\x64" /LIBPATH:"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.3\lib\x64" OGLTemplate.obj OGLTemplate.res user32.lib kernel32.lib gdi32.lib

OGLTemplate.exe

