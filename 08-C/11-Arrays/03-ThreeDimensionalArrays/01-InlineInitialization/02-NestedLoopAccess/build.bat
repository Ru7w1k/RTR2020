@echo off

set name=array3D
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe
