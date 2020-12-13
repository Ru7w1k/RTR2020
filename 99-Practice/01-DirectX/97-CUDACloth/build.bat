cls

rc.exe main.rc

nvcc.exe -c -o sinwaveVBO.obj sinewave_VBO.cu

cl.exe /c /EHsc /I"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.1\include" main.cpp

link.exe /LIBPATH:"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.1\lib\x64" sinwaveVBO.obj main.obj main.res user32.lib kernel32.lib gdi32.lib /SUBSYSTEM:WINDOWS

sinwaveVBO.exe
