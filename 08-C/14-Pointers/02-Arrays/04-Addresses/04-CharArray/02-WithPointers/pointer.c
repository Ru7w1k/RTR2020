#include <stdio.h>

int main(void)
{
    // variable declarations
    char cArray_rc[10];
    char *ptr_cArray_rc = NULL;
    int i_rc;

    // code
    for (i_rc = 0; i_rc < 10; i_rc++)
        cArray_rc[i_rc] = (char)(i_rc + 65);

    // name of the array is its base address
    // hence, 'cArray' is the base address of cArray[]
    // or cArray is the address of cArray[0]

    ptr_cArray_rc = cArray_rc; // ptr_cArray_rc = &cArray_rc[0]
    
    printf("\n\n");
    printf("Elements of the Character array: \n\n");
    for (i_rc = 0; i_rc < 10; i_rc++)
        printf("cArray[%d] = %c\n", i_rc, *(ptr_cArray_rc + i_rc));

    printf("Elements of the Character array: \n\n");
    for (i_rc = 0; i_rc < 10; i_rc++)
        printf("cArray[%d] = %c \t\tAddress = %p\n", i_rc, *(ptr_cArray_rc + i_rc), (ptr_cArray_rc + i_rc));

    printf("\n\n");

    return(0);
}