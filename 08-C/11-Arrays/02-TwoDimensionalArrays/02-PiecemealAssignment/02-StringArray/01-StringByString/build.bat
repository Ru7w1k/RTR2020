@echo off

set name=array2D
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe
