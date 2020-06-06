@echo off

set name=doWhileLoop
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe