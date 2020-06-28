#include <stdio.h>
#include <stdlib.h> // contains prototypes of malloc() and free()

#define NUM_ROWS 5
#define NUM_COLS 3

int main(void)
{
    // variable declarations
    int i_rc, j_rc;
    int **ptr_iArray_rc = NULL;

    // code
    printf("\n\n");

    // memory allocation
    ptr_iArray_rc = (int **)malloc(NUM_ROWS * sizeof(int *));

    if (ptr_iArray_rc == NULL)
    {
        printf("MEMORY ALLOCATION FAILED FOR 1D ARRAY OF BASE ADDRESSES OF %d ROWS! Exiting now..\n\n", NUM_ROWS);
        exit(0);
    }
    else
    {
        printf("MEMORY ALLOCATION SUCCESSED FOR 1D ARRAY OF BASE ADDRESSES OF %d ROWS!\n\n", NUM_ROWS);
    }
    
    // allocating memory to each row
    for (i_rc = 0; i_rc < NUM_ROWS; i_rc++)
    {
        ptr_iArray_rc[i_rc] = (int *)malloc(sizeof(int) * NUM_COLS);
        if (ptr_iArray_rc[i_rc] == NULL)
        {
            printf("MEMORY ALLOCATION TO THE COLUMNS OF ROW %d FAILED!! Exiting Now..\n\n", i_rc);
            exit(0);
        }
        else
        {
            printf("MEMORY ALLOCATION TO THE COLUMNS OF ROW %d HAS SUCCEEDED!!\n\n", i_rc);
        }
        
    }


    for (i_rc = 0; i_rc < NUM_ROWS; i_rc++)
    {
        for (j_rc = 0; j_rc < NUM_COLS; j_rc++)
        {
            *(*(ptr_iArray_rc + i_rc) + j_rc) = (i_rc + 1) * (j_rc + 1);
        }
    }

    // DISPLAYING VALUES
    printf("\n\n");
    printf("2D Integer array elements along with addresses: \n\n");
    for (i_rc = 0; i_rc < NUM_ROWS; i_rc++)
    {
        for (j_rc = 0; j_rc < NUM_COLS; j_rc++)
        {
            printf("ptr_iArray[%d][%d] = %d \t\t at address &ptr_iArray[%d][%d]: %p\n", i_rc, j_rc, ptr_iArray_rc[i_rc][j_rc], i_rc, j_rc, &ptr_iArray_rc[i_rc][j_rc]);
        }
        printf("\n\n");
    }

    // FREEING ALLOCATED MEMEORY
    // FREEING MEMORY OF EACH ROW
    for (i_rc = (NUM_ROWS - 1); i_rc >= 0; i_rc--)
    {
        if (*(ptr_iArray_rc + i_rc)) // if (ptr_iArray[i])
        {
            free(*(ptr_iArray_rc + i_rc));
            *(ptr_iArray_rc + i_rc) = NULL;
            printf("MEMORY ALLOCATED FOR ROW %d HAS BEEN SUCCESSFULLY FREED!\n\n", i_rc);
        }
    }

    // FREEING MEMORY OF ptr_iArray
    if (ptr_iArray_rc)
    {
        free(ptr_iArray_rc);
        ptr_iArray_rc = NULL;
        printf("MEMORY ALLOCATED FOR ptr_iArray HAS BEEN SUCCESSFULLY FREED!\n\n");
    }

    return(0);
}
