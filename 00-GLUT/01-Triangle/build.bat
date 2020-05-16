REM compile
cl.exe /c /EHsc /I C:\freeglut\include glut-triangle.cpp

REM link
link.exe glut-triangle.obj /LIBPATH:C:\freeglut\lib /SUBSYSTEM:CONSOLE

REM execute
glut-triangle.exe