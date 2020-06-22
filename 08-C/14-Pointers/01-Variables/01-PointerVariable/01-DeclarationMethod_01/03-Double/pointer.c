#include <stdio.h>

int main(void)
{
    // variable declarations
    double num_rc;
    double *ptr_rc = NULL; // declaration method 1: *ptr is a variable of type double

    // code
    num_rc = 5.1521312324;

    printf("\n\n");

    printf("BEFORE ptr = & num\n\n");
    printf("Value of num            = %lf\n\n", num_rc);
    printf("Address of num          = %p\n\n", &num_rc);
    printf("Value at address of num = %lf\n\n", *(&num_rc));

    // assigning address of variable num to pointer variable ptr
    ptr_rc = &num_rc;

    printf("\n\n");

    printf("AFTER ptr = & num\n\n");
    printf("Value of num            = %lf\n\n", num_rc);
    printf("Address of num          = %p\n\n", ptr_rc);
    printf("Value at address of num = %lf\n\n", *ptr_rc);

    return(0);
}