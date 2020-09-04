cls

nvcc.exe -c -o cloth.obj cloth.cu

cl.exe /c /EHsc /I"C:\glew\include" /I"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.1\include" main.cpp

link.exe /LIBPATH:"C:\glew\lib\Release\x64" /LIBPATH:"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.1\lib\x64" main.obj cloth.obj opengl32.lib user32.lib kernel32.lib gdi32.lib

main.exe