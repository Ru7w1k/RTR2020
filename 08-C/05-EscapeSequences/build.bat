@echo off

set name=escapeSequences
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe