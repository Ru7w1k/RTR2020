#include <stdio.h>
#include <stdlib.h>

struct MyData
{
    int *ptr_i;
    int i;

    float *ptr_f;
    float f;

    double *ptr_d;
    double d;
};

int main(void)
{
    // variable declarations
    struct MyData *pData_rc = NULL;

    // code
    printf("\n\n");
    pData_rc = (struct MyData *)malloc(sizeof(struct MyData));
    if (pData_rc == NULL)
    {
        printf("FAILED TO ALLOCATE MEMORY TO struct MyData! exiting now..\n\n");
        exit(0);
    }
    else
    {
        printf("Successfully allocated memory to struct MyData!\n\n");
    }

    (*pData_rc).i = 9;
    (*pData_rc).ptr_i = &(*pData_rc).i;

    (*pData_rc).f = 16.546f;
    (*pData_rc).ptr_f = &(*pData_rc).f;

    (*pData_rc).d = 96.48651384;
    (*pData_rc).ptr_d = &(*pData_rc).d;

    printf("\n\n");
    printf("i = %d\n", *((*pData_rc).ptr_i));
    printf("Address of i = %p\n", (*pData_rc).ptr_i);
    
    printf("\n\n");
    printf("f = %f\n", *((*pData_rc).ptr_f));
    printf("Address of f = %p\n", (*pData_rc).ptr_f);
    
    printf("\n\n");
    printf("d = %lf\n", *((*pData_rc).ptr_d));
    printf("Address of d = %p\n", (*pData_rc).ptr_d);

    if (pData_rc)
    {
        free(pData_rc);
        pData_rc = NULL;
        printf("MEMORY ALLOCATED TO struct MyData HAS BEEN FREED!\n\n");
    }

    return(0);
}
