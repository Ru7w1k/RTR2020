#include <stdio.h>

int main(void)
{
    // variable declarations
    float num_rc;
    float *ptr_rc = NULL; // declaration method 1: *ptr is a variable of type float

    // code
    num_rc = 65.15f;

    printf("\n\n");

    printf("BEFORE ptr = & num\n\n");
    printf("Value of num            = %f\n\n", num_rc);
    printf("Address of num          = %p\n\n", &num_rc);
    printf("Value at address of num = %f\n\n", *(&num_rc));

    // assigning address of variable num to pointer variable ptr
    ptr_rc = &num_rc;

    printf("\n\n");

    printf("AFTER ptr = & num\n\n");
    printf("Value of num            = %f\n\n", num_rc);
    printf("Address of num          = %p\n\n", ptr_rc);
    printf("Value at address of num = %f\n\n", *ptr_rc);

    return(0);
}