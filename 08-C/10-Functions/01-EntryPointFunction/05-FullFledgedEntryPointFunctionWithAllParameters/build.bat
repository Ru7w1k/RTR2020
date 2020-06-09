@echo off

set name=function
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe
