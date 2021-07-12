cl.exe /c /EHsc /I"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.3\include" HelloOpenCL.cpp

link.exe /LIBPATH:"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.3\lib\x64" HelloOpenCL.obj opencl.lib 

HelloOpenCL.exe
