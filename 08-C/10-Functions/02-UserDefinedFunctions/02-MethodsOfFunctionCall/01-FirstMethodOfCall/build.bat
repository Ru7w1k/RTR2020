@echo off

set name=callMethod
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe
