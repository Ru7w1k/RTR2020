@echo off

set name=string
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe
