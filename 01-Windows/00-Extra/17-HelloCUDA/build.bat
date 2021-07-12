nvcc.exe -c -o HelloCUDA.obj HelloCUDA.cu

link.exe /LIBPATH:"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.3\lib\x64" HelloCUDA.obj user32.lib kernel32.lib gdi32.lib cudart.lib

HelloCUDA.exe
