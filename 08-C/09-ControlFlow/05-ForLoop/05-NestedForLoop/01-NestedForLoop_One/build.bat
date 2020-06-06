@echo off

set name=forLoop
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe