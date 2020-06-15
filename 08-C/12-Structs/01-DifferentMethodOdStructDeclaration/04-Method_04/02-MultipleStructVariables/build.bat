@echo off

set name=struct
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe
