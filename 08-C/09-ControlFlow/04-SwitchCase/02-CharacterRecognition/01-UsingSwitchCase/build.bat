@echo off

set name=switchCase
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe