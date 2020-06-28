#include <stdio.h>
#include <stdlib.h> // contains prototypes of malloc() and free()

#define NUM_ROWS 5
#define NUM_COLS 5

int main(void)
{
    // variable declarations
    int *iArray_rc[NUM_ROWS];
    int i_rc, j_rc;

    // code

    printf("\n\n");
    printf("First Memory allocation for 2D integer array\n\n");
    for (i_rc = 0; i_rc < NUM_ROWS; i_rc++)
    {
        // row 0 will have (NUM_COLS - 0) = (5 - 0) = 5 columns..
        // row 1 will have (NUM_COLS - 1) = (5 - 1) = 4 columns..
        // row 2 will have (NUM_COLS - 2) = (5 - 2) = 3 columns..
        // row 3 will have (NUM_COLS - 3) = (5 - 3) = 2 columns..
        // row 4 will have (NUM_COLS - 4) = (5 - 4) = 1 columns..

        iArray_rc[i_rc] = (int *)malloc(sizeof(int) * NUM_COLS - 1);
        if (iArray_rc[i_rc] == NULL)
        {
            printf("MEMORY ALLOCATION TO THE COLUMNS OF ROW %d FAILED!! Exiting Now..\n\n", i_rc);
            exit(0);
        }
        else
        {
            printf("MEMORY ALLOCATION TO THE COLUMNS OF ROW %d HAS SUCCEEDED!!\n\n", i_rc);
        }
        
    }

    // ASSIGNING VALUES TO 2D ARRAY
    for (i_rc = 0; i_rc < NUM_ROWS; i_rc++)
    {
        for (j_rc = 0; j_rc < (NUM_COLS - i_rc); j_rc++)
        {
            iArray_rc[i_rc][j_rc] = (i_rc + 1) * (j_rc + 1);
        }
    }

    // DISPLAYING 2D ARRAY
    for (i_rc = 0; i_rc < NUM_ROWS; i_rc++)
    {
        for (j_rc = 0; j_rc < (NUM_COLS - i_rc); j_rc++)
        {
            printf("iArray[%d][%d] = %d \t at address: %p\n", i_rc, j_rc, iArray_rc[i_rc][j_rc], &iArray_rc[i_rc][j_rc]);
        }
        printf("\n");
    }

    // FREEING ALLOCATED MEMORY TO 2D ARRAY (MUST BE DONE IN REVERSE ORDER)
    for (i_rc = (NUM_ROWS - 1); i_rc >= 0; i_rc--)
    {
        if (iArray_rc[i_rc])
        {
            free(iArray_rc[i_rc]);
            iArray_rc[i_rc] = NULL;
            printf("MEMORY ALLOCATED FOR ROW %d OF 2D INTEGER ARRAY HAS BEEN SUCCESSFULLY FREED!\n\n", i_rc);
        }
    }

    return(0);
}
