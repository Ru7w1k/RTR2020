@echo off

set name=break
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe