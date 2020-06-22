#include <stdio.h>

int main(void)
{
    // variable declarations
    char ch_rc;
    char* ptr_rc = NULL; // declaration method 1: ptr is a variable of type char*

    // code
    ch_rc = 'R';

    printf("\n\n");

    printf("BEFORE ptr = & num\n\n");
    printf("Value of num            = %c\n\n", ch_rc);
    printf("Address of num          = %p\n\n", &ch_rc);
    printf("Value at address of num = %c\n\n", *(&ch_rc));

    // assigning address of variable num to pointer variable ptr
    ptr_rc = &ch_rc;

    printf("\n\n");

    printf("AFTER ptr = & num\n\n");
    printf("Value of num            = %c\n\n", ch_rc);
    printf("Address of num          = %p\n\n", ptr_rc);
    printf("Value at address of num = %c\n\n", *ptr_rc);

    return(0);
}