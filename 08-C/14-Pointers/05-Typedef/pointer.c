#include <stdio.h>
#include <stdlib.h>

struct MyData
{
    int i;
    float f;
    double d;
};

int main(void)
{
    // variable declarations
    int i_size_rc;
    int f_size_rc;
    int d_size_rc;
    int struct_MyData_size_rc;
    int pointer_to_struct_MyData_size_rc;

    typedef struct MyData* MyDataPtr;

    MyDataPtr pData_rc;

    // code
    printf("\n\n");

    pData_rc = (MyDataPtr)malloc(sizeof(struct MyData));
    if (pData_rc == NULL)
    {
        printf("FAILED TO ALLOCATE MEMORY TO struct MyData! exiting now..\n\n");
        exit(0);
    }
    else
    {
        printf("Successfully allocated memory to struct MyData!\n\n");
    }
    
    // assigning data values to the data members if struct MyData
    pData_rc->i = 20;
    pData_rc->f = 2.50f;
    pData_rc->d = 84.4684320;

    // displaying values of the data member of struct MyData
    printf("\n\n");
    printf("DATA MEMBERS OF struct MyData ARE: \n\n");
    printf("i = %d\n", pData_rc->i);
    printf("f = %f\n", pData_rc->f);
    printf("d = %lf\n", pData_rc->d);

    // calculating size (in bytes) of the data members of struct MyData
    i_size_rc = sizeof(pData_rc->i);
    f_size_rc = sizeof(pData_rc->f);
    d_size_rc = sizeof(pData_rc->d);

    // displaying sizes (in bytes) of the data members of struct MyData
    printf("\n\n");
    printf("SIZES (in bytes) OF DATA MEMBERS OF struct MyData ARE: \n\n");
    printf("Size of i = %d bytes\n", i_size_rc);
    printf("Size of f = %d bytes\n", f_size_rc);
    printf("Size of d = %d bytes\n", d_size_rc);

    // calculating size (in bytes) of the entire struct MyData
    struct_MyData_size_rc = sizeof(struct MyData);
    pointer_to_struct_MyData_size_rc = sizeof(MyDataPtr);

    // displaying sizes (in bytes) of the entire struct MyData
    printf("\n\n");
    printf("Size of struct MyData: %d bytes\n\n", struct_MyData_size_rc);
    printf("Size of pointer to struct MyData: %d bytes\n\n", pointer_to_struct_MyData_size_rc);

    if (pData_rc)
    {
        free(pData_rc);
        pData_rc = NULL;
        printf("MEMORY ALLOCATED TO struct MyData HAS BEEN FREED!\n\n");
    }

    return(0);
}
