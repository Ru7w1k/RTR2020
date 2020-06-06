@echo off

set name=ifElseLadder
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe