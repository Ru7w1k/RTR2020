@echo off

set name=ifStatement
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe