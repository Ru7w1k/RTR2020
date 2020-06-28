#include <stdio.h>

int main(void)
{
    // variable declarations
    int num_rc;
    int *ptr_rc = NULL;
    int **pptr_rc = NULL; // **pptr is of type int

    // code
    num_rc = 20;

    printf("\n\n");

    printf("BEFORE ptr = &num: \n\n");
    printf("value of 'num'            = %d\n\n", num_rc);
    printf("Address of 'num'          = %p\n\n", &num_rc);
    printf("value at Address of 'num' = %d\n\n", *(&num_rc));

    ptr_rc = &num_rc;
    printf("\n\n");

    printf("AFTER ptr = &num: \n\n");
    printf("value of 'num'            = %d\n\n", num_rc);
    printf("Address of 'num'          = %p\n\n", ptr_rc);
    printf("value at Address of 'num' = %d\n\n", *ptr_rc);

    // assigning address of ptr to pointer-to-pointer variable pptr
    pptr_rc = &ptr_rc;

    printf("\n\n");

    printf("AFTER pptr = &ptr: \n\n");
    printf("value of 'num'                           = %d\n\n", num_rc);
    printf("Address of 'num' (ptr)                   = %p\n\n", ptr_rc);
    printf("Address of 'ptr' (pptr)                  = %p\n\n", pptr_rc);
    printf("value at Address of 'ptr' (*pptr)        = %p\n\n", *pptr_rc);
    printf("value at Address of 'num' (*ptr) (*pptr) = %d\n\n", **pptr_rc);

    return(0);
}