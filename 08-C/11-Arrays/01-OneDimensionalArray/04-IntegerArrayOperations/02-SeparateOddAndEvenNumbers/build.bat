@echo off

set name=array
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe
