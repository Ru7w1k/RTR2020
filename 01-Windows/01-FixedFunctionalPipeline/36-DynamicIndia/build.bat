cl.exe /c /EHsc OGLTemplate.cpp

rc.exe OGLTemplate.rc

link.exe OGLTemplate.obj OGLTemplate.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

OGLTemplate.exe
