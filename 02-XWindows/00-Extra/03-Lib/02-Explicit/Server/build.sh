g++ -Wall -fPIC -c -o mylib.obj mylib.cpp
g++ -shared -o libmylib.so mylib.obj

