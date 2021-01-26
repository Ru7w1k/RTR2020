#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "mylib.h"

typedef int(*addFunc_t)(int, int);
addFunc_t addFunc;

int main()
{
    int res;
    void *handle;
    char *error;

    handle = dlopen("./libmylib.so", RTLD_LAZY);
    if (!handle)
	{
		printf("\nERROR: dlload: %s", dlerror());
		return(EXIT_FAILURE);
	}

	dlerror(); // clear existing error

    addFunc = (int(*)(int, int)) dlsym(handle, "add");
	error = dlerror();
	if (error != NULL)
	{
		printf("\nERROR: dlsym: %s", dlerror());
		return false;
	}

    res = addFunc(10, 14);
    printf("%d\n", res);
    return(0);
}