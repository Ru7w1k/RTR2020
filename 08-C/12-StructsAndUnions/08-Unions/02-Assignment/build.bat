@echo off

set name=union
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe
