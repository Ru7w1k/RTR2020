cls

del cloth.obj main.obj main.exe log.txt

rc.exe resource.rc

nvcc.exe -c -o main.obj main.cu

cl.exe /c /EHsc /I"C:\glew\include" /I"C:\freetype" text.cpp 

link.exe /LIBPATH:"C:\glew\lib\Release\x64" /LIBPATH:"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.1\lib\x64" /LIBPATH:"C:\freetype\lib\win64" main.obj  resource.res text.obj opengl32.lib user32.lib kernel32.lib gdi32.lib

main.exe