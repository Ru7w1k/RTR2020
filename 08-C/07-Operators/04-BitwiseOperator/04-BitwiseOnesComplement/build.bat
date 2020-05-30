@echo off

set name=operators
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe