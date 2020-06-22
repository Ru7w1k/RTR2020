#include <stdio.h>

int main(void)
{
    // variable declarations
    float fArray_rc[10];
    float *ptr_fArray_rc = NULL;
    int i_rc;

    // code
    for (i_rc = 0; i_rc < 10; i_rc++)
        fArray_rc[i_rc] = (float)(i_rc + 1) * 4.5f;

    // name of the array is its base address
    // hence, 'fArray' is the base address of fArray[]
    // or fArray is the address of fArray[0]

    ptr_fArray_rc = fArray_rc; // ptr_fArray_rc = &fArray_rc[0]
    
    printf("\n\n");
    printf("Elements of the Float array: \n\n");
    for (i_rc = 0; i_rc < 10; i_rc++)
        printf("fArray[%d] = %f\n", i_rc, *(ptr_fArray_rc + i_rc));

    printf("Elements of the Float array: \n\n");
    for (i_rc = 0; i_rc < 10; i_rc++)
        printf("fArray[%d] = %f \t\tAddress = %p\n", i_rc, *(ptr_fArray_rc + i_rc), (ptr_fArray_rc + i_rc));

    printf("\n\n");

    return(0);
}