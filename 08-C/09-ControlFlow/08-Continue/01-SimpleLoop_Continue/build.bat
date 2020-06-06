@echo off

set name=continue
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe