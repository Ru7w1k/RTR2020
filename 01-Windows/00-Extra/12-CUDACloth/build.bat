cls

del cloth.obj main.obj main.exe log.txt

rc.exe resource.rc

nvcc.exe -c -o main.obj main.cu

rem cl.exe /c /EHsc /I"C:\glew\include" /I"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.1\include" main.cpp

link.exe /LIBPATH:"C:\glew\lib\Release\x64" /LIBPATH:"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.1\lib\x64" main.obj  resource.res opengl32.lib user32.lib kernel32.lib gdi32.lib

main.exe