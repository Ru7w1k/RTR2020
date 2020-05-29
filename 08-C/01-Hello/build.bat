@echo off

set name=hello
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe