@echo off

set name=primitiveDataTypeSizes
cl.exe /c /EHsc %name%.c
link.exe %name%.obj
%name%.exe