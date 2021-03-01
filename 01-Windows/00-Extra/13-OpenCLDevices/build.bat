cls

cl.exe /c /EHsc /I"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.1\include" EnumOpenCLDevices.c

link.exe /LIBPATH:"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.1\lib\x64" EnumOpenCLDevices.obj opencl.lib 

EnumOpenCLDevices.exe