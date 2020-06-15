@echo off

set name=typedefs
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe
