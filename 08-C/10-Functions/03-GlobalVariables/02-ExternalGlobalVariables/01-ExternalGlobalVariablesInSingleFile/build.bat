@echo off

set name=globalVariables
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe
