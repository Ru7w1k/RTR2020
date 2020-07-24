REM compile
cl.exe /c /EHsc /I C:\freeglut\include glut-D.cpp

REM link
link.exe glut-D.obj /LIBPATH:C:\freeglut\lib /SUBSYSTEM:CONSOLE

REM execute
glut-D.exe