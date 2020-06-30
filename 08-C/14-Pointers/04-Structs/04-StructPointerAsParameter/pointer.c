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
    // function declarations
    void ChangeValues(struct MyData *);

    // variable declarations
    struct MyData* pData_rc;

    // code
    printf("\n\n");

    pData_rc = (struct MyData*)malloc(sizeof(struct MyData));
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

    ChangeValues(pData_rc);

    // displaying values of the data member of struct MyData
    printf("\n\n");
    printf("DATA MEMBERS OF struct MyData ARE: \n\n");
    printf("i = %d\n", pData_rc->i);
    printf("f = %f\n", pData_rc->f);
    printf("d = %lf\n", pData_rc->d);

    if (pData_rc)
    {
        free(pData_rc);
        pData_rc = NULL;
        printf("MEMORY ALLOCATED TO struct MyData HAS BEEN FREED!\n\n");
    }

    return(0);
}

void ChangeValues(struct MyData *pData)
{
    // code
    pData->i = 40;
    pData->f = 6.5460f;
    pData->d = 14.89543573243;
}

