@echo off

set name=constants
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe