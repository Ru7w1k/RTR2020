@echo off

set name=vaargs
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe
