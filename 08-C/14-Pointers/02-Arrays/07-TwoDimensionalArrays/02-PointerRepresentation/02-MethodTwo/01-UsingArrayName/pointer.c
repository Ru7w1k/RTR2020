#include <stdio.h>
#include <stdlib.h> // contains prototypes of malloc() and free()

#define NUM_ROWS 5
#define NUM_COLS 3

int main(void)
{
    // variable declarations
    int iArray_rc[NUM_ROWS][NUM_COLS];
    int i_rc, j_rc;

    // code
    for (i_rc = 0; i_rc < NUM_ROWS; i_rc++)
    {
        for (j_rc = 0; j_rc < NUM_COLS; j_rc++)
        {
            *(*(iArray_rc + i_rc) + j_rc) = (i_rc + 1) * (j_rc + 1); // iArray[i] can be treated as 1D array using pointer
        }
    }

    printf("\n\n");
    printf("2D Integer array elements along with addresses: \n\n");
    for (i_rc = 0; i_rc < NUM_ROWS; i_rc++)
    {
        for (j_rc = 0; j_rc < NUM_COLS; j_rc++)
        {
            printf("*(*(iArray + %d) + %d) = %d \t\t at address *(*(iArray + %d) + %d): %p\n", i_rc, j_rc, *(*(iArray_rc + i_rc) + j_rc), i_rc, j_rc, (*(iArray_rc + i_rc) + j_rc));
        }
        printf("\n\n");
    }

    return(0);
}
