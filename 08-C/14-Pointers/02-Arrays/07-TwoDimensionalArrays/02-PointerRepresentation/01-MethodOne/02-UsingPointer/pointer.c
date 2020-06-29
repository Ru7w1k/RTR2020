#include <stdio.h>
#include <stdlib.h> // contains prototypes of malloc() and free()

#define NUM_ROWS 5
#define NUM_COLS 3

int main(void)
{
    // variable declarations
    int iArray_rc[NUM_ROWS][NUM_COLS];
    int i_rc, j_rc;

    int *ptr_iArray_row = NULL;

    // code
    for (i_rc = 0; i_rc < NUM_ROWS; i_rc++)
    {
        ptr_iArray_row = iArray_rc[i_rc]; // iArray[i] is the base address of ith row

        for (j_rc = 0; j_rc < NUM_COLS; j_rc++)
        {
            *(ptr_iArray_row + j_rc) = (i_rc + 1) * (j_rc + 1); // ptr_iArray_row (that is, iArray[i] can be treaded as 1D array using pointers)
        }
    }

    printf("\n\n");
    printf("2D Integer array elements along with addresses: \n\n");
    for (i_rc = 0; i_rc < NUM_ROWS; i_rc++)
    {
        ptr_iArray_row = iArray_rc[i_rc];
        for (j_rc = 0; j_rc < NUM_COLS; j_rc++)
        {
            printf("*(ptr_iArray_row + %d) = %d \t\t at address (ptr_iArray_row + j): %p\n", j_rc, *(ptr_iArray_row + j_rc), (ptr_iArray_row + j_rc));
        }
        printf("\n\n");
    }

    return(0);
}
