#include <stdio.h>

#include "mylib.h"

extern "C" int add(int a, int b)
{
    return (a+b);
}
