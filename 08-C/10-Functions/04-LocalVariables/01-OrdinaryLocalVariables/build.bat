@echo off

set name=localVariables
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe
