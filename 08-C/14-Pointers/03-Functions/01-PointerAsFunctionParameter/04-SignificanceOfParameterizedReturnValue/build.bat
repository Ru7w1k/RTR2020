@echo off

set name=functionPointer
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe
