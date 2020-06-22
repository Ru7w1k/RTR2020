#include <stdio.h>

int main(void)
{
    // variable declarations
    float fArray_rc[10];
    int i_rc = 0;

    // code
    for (i_rc = 0; i_rc < 10; i_rc++)
        fArray_rc[i_rc] = (float)(i_rc + 1) * 4.5f;

    printf("\n\n");
    printf("Elements of the Float array: \n\n");
    for (i_rc = 0; i_rc < 10; i_rc++)
        printf("fArray[%d] = %f\n", i_rc, fArray_rc[i_rc]);

    printf("Elements of the Float array: \n\n");
    for (i_rc = 0; i_rc < 10; i_rc++)
        printf("fArray[%d] = %f \t\tAddress = %p\n", i_rc, fArray_rc[i_rc], &fArray_rc[i_rc]);

    printf("\n\n");

    return(0);
}