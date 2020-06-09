@echo off

set name=definitionMethod
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe
