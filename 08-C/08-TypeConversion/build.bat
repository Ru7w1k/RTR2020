@echo off

set name=typeConversion
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe