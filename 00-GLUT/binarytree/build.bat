REM compile
cl.exe /c /EHsc /I C:\freeglut\include btree.cpp

REM link
link.exe btree.obj /LIBPATH:C:\freeglut\lib /SUBSYSTEM:CONSOLE

REM execute
btree.exe