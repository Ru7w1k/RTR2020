#include <stdio.h>

int main(void)
{
    // variable declarations
    char cArray_rc[10];
    int i_rc = 0;

    // code
    for (i_rc = 0; i_rc < 10; i_rc++)
        cArray_rc[i_rc] = (char)(i_rc + 65) ;

    printf("\n\n");
    printf("Elements of the Character array: \n\n");
    for (i_rc = 0; i_rc < 10; i_rc++)
        printf("cArray[%d] = %c\n", i_rc, cArray_rc[i_rc]);

    printf("Elements of the Character array: \n\n");
    for (i_rc = 0; i_rc < 10; i_rc++)
        printf("cArray[%d] = %c \t\tAddress = %p\n", i_rc, cArray_rc[i_rc], &cArray_rc[i_rc]);

    printf("\n\n");

    return(0);
}