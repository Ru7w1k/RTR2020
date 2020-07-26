REM compile
cl.exe /c /EHsc main.cpp

REM link
link.exe main.obj user32.lib gdi32.lib kernel32.lib /SUBSYSTEM:WINDOWS

REM execute
main.exe