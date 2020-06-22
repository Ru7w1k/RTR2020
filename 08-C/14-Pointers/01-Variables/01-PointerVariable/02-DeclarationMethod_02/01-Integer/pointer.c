#include <stdio.h>

int main(void)
{
    // variable declarations
    int num_rc;
    int* ptr_rc = NULL; // declaration method 1: ptr is a variable of type int*

    // code
    num_rc = 10;

    printf("\n\n");

    printf("BEFORE ptr = & num\n\n");
    printf("Value of num            = %d\n\n", num_rc);
    printf("Address of num          = %p\n\n", &num_rc);
    printf("Value at address of num = %d\n\n", *(&num_rc));

    // assigning address of variable num to pointer variable ptr
    ptr_rc = &num_rc;

    printf("\n\n");

    printf("AFTER ptr = & num\n\n");
    printf("Value of num            = %d\n\n", num_rc);
    printf("Address of num          = %p\n\n", ptr_rc);
    printf("Value at address of num = %d\n\n", *ptr_rc);

    return(0);
}