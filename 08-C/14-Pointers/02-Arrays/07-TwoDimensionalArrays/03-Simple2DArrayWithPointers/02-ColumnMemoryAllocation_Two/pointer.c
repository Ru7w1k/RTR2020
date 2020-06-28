#include <stdio.h>
#include <stdlib.h> // contains prototypes of malloc() and free()

#define NUM_ROWS 5
#define NUM_COLS_ONE 3
#define NUM_COLS_TWO 8

int main(void)
{
    // variable declarations
    int *iArray_rc[NUM_ROWS];
    int i_rc, j_rc;

    // code

    // **** ONE (ALLOCATING MEMORY FOR AN ARRAY OF 3 INTEGERS PER ROW) ****
    printf("\n\n");
    printf("First Memory allocation for 2D integer array\n\n");
    for (i_rc = 0; i_rc < NUM_ROWS; i_rc++)
    {
        iArray_rc[i_rc] = (int *)malloc(sizeof(int) * NUM_COLS_ONE);
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
        for (j_rc = 0; j_rc < NUM_COLS_ONE; j_rc++)
        {
            iArray_rc[i_rc][j_rc] = (i_rc + 1) * (j_rc + 1);
        }
    }

    // DISPLAYING 2D ARRAY
    printf("\n\n");
    printf("DISPLAYING 2D ARRAYS: \n\n");
    for (i_rc = 0; i_rc < NUM_ROWS; i_rc++)
    {
        for (j_rc = 0; j_rc < NUM_COLS_ONE; j_rc++)
        {
            printf("iArray[%d][%d] = %d\n", i_rc, j_rc, iArray_rc[i_rc][j_rc]);
        }
        printf("\n\n");
    }
    printf("\n\n");

    // FREEING ALLOCATED MEMORY TO 2D ARRAY (MUST BE DONE IN REVERSE ORDER)
    for (i_rc = (NUM_ROWS - 1); i_rc >= 0; i_rc--)
    {
        free(iArray_rc[i_rc]);
        iArray_rc[i_rc] = NULL;
        printf("MEMORY ALLOCATED FOR ROW %d OF 2D INTEGER ARRAY HAS BEEN SUCCESSFULLY FREED!\n\n", i_rc);
    }

    // **** TWO (ALLOCATING MEMORY FOR AN ARRAY OF 8 INTEGERS PER ROW) ****
    printf("\n\n");
    printf("First Memory allocation for 2D integer array\n\n");
    for (i_rc = 0; i_rc < NUM_ROWS; i_rc++)
    {
        iArray_rc[i_rc] = (int *)malloc(sizeof(int) * NUM_COLS_TWO);
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
        for (j_rc = 0; j_rc < NUM_COLS_TWO; j_rc++)
        {
            iArray_rc[i_rc][j_rc] = (i_rc + 1) * (j_rc + 1);
        }
    }

    // DISPLAYING 2D ARRAY
    printf("\n\n");
    printf("DISPLAYING 2D ARRAYS: \n\n");
    for (i_rc = 0; i_rc < NUM_ROWS; i_rc++)
    {
        for (j_rc = 0; j_rc < NUM_COLS_TWO; j_rc++)
        {
            printf("iArray[%d][%d] = %d\n", i_rc, j_rc, iArray_rc[i_rc][j_rc]);
        }
        printf("\n\n");
    }
    printf("\n\n");

    // FREEING ALLOCATED MEMORY TO 2D ARRAY (MUST BE DONE IN REVERSE ORDER)
    for (i_rc = (NUM_ROWS - 1); i_rc >= 0; i_rc--)
    {
        free(iArray_rc[i_rc]);
        iArray_rc[i_rc] = NULL;
        printf("MEMORY ALLOCATED FOR ROW %d OF 2D INTEGER ARRAY HAS BEEN SUCCESSFULLY FREED!\n\n", i_rc);
    }

    return(0);
}
