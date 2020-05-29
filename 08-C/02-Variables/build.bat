@echo off

set name=variables
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe