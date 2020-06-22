#include <stdio.h>

int main(void)
{
    // variable declarations
    int iArray_rc[10];
    int *ptr_iArray_rc = NULL;
    int i_rc;

    // code
    for (i_rc = 0; i_rc < 10; i_rc++)
        iArray_rc[i_rc] = (i_rc + 1) * 4;

    // name of the array is its base address
    // hence, 'iArray' is the base address of iArray[]
    // or iArray is the address of iArray[0]

    ptr_iArray_rc = iArray_rc; // ptr_iArray_rc = &iArray_rc[0]
    
    printf("\n\n");
    printf("Elements of the Integer array: \n\n");
    for (i_rc = 0; i_rc < 10; i_rc++)
        printf("iArray[%d] = %d\n", i_rc, *(ptr_iArray_rc + i_rc));

    printf("Elements of the Integer array: \n\n");
    for (i_rc = 0; i_rc < 10; i_rc++)
        printf("iArray[%d] = %4d \t\tAddress = %p\n", i_rc, *(ptr_iArray_rc + i_rc), (ptr_iArray_rc + i_rc));

    printf("\n\n");

    return(0);
}