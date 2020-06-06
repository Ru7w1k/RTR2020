@echo off

set name=ifElseStatement
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe