@echo off

:: Compile Source Code
cl.exe /c /EHsc MyWindow.cpp

:: Compile Resource Script
rc.exe MyWindow.rc

:: Link
link.exe MyWindow.obj MyWindow.res user32.lib gdi32.lib

:: Execute
MyWindow.exe

