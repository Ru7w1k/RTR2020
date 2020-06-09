@echo off

set name=globalVariables
cl.exe /c /EHsc %name%.c file1.c file2.c
link.exe %name%.obj file1.obj file2.obj
%name%.exe
