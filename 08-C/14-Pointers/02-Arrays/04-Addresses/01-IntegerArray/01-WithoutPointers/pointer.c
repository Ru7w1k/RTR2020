#include <stdio.h>

int main(void)
{
    // variable declarations
    int iArray_rc[10];
    int i_rc = 0;

    // code
    for (i_rc = 0; i_rc < 10; i_rc++)
        iArray_rc[i_rc] = (i_rc + 1) * 4;

    printf("\n\n");
    printf("Elements of the Integer array: \n\n");
    for (i_rc = 0; i_rc < 10; i_rc++)
        printf("iArray[%d] = %d\n", i_rc, iArray_rc[i_rc]);

    printf("Elements of the Integer array: \n\n");
    for (i_rc = 0; i_rc < 10; i_rc++)
        printf("iArray[%d] = %4d \t\tAddress = %p\n", i_rc, iArray_rc[i_rc], &iArray_rc[i_rc]);

    printf("\n\n");

    return(0);
}