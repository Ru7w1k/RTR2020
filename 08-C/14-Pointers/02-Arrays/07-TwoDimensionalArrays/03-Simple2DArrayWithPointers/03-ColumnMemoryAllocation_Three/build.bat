@echo off

set name=pointer
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe
