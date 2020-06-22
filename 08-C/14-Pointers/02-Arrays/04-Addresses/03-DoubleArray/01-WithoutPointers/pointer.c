#include <stdio.h>

int main(void)
{
    // variable declarations
    double dArray_rc[10];
    int i_rc = 0;

    // code
    for (i_rc = 0; i_rc < 10; i_rc++)
        dArray_rc[i_rc] = (double)(i_rc + 1) * 2.84354786535;

    printf("\n\n");
    printf("Elements of the Double array: \n\n");
    for (i_rc = 0; i_rc < 10; i_rc++)
        printf("dArray[%d] = %lf\n", i_rc, dArray_rc[i_rc]);

    printf("Elements of the Double array: \n\n");
    for (i_rc = 0; i_rc < 10; i_rc++)
        printf("dArray[%d] = %lf \t\tAddress = %p\n", i_rc, dArray_rc[i_rc], &dArray_rc[i_rc]);

    printf("\n\n");

    return(0);
}