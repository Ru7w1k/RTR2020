@echo off

set name=ifElseIfLadder
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe