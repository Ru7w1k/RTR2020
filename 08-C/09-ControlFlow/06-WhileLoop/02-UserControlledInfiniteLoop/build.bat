@echo off

set name=whileLoop
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe