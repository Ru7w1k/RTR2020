#include <stdio.h>

int main(void)
{
    // variable declarations
    double dArray_rc[10];
    double *ptr_dArray_rc = NULL;
    int i_rc;

    // code
    for (i_rc = 0; i_rc < 10; i_rc++)
        dArray_rc[i_rc] = (double)(i_rc + 1) * 4.5846534864;

    // name of the array is its base address
    // hence, 'dArray' is the base address of dArray[]
    // or dArray is the address of dArray[0]

    ptr_dArray_rc = dArray_rc; // ptr_dArray_rc = &dArray_rc[0]
    
    printf("\n\n");
    printf("Elements of the Double array: \n\n");
    for (i_rc = 0; i_rc < 10; i_rc++)
        printf("dArray[%d] = %lf\n", i_rc, *(ptr_dArray_rc + i_rc));

    printf("Elements of the Double array: \n\n");
    for (i_rc = 0; i_rc < 10; i_rc++)
        printf("dArray[%d] = %lf \t\tAddress = %p\n", i_rc, *(ptr_dArray_rc + i_rc), (ptr_dArray_rc + i_rc));

    printf("\n\n");

    return(0);
}