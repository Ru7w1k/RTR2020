@echo off

set name=formatStrings
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe