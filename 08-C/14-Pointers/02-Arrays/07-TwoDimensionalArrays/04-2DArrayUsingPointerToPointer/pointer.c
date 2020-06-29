#include <stdio.h>
#include <stdlib.h> 

int main(void)
{
    // variable declarations
    int **ptr_iArray_rc = NULL;
    int i_rc, j_rc;
    int num_rows_rc, num_cols_rc;

    // code

    // accept number of rows from user
    printf("\n\n");
    printf("Enter Number of Rows: ");
    scanf("%d", &num_rows_rc);

    // accept number of columns from user
    printf("\n\n");
    printf("Enter Number of Columns: ");
    scanf("%d", &num_cols_rc);

    // allocating memory to 1D array consisting of base address of rows
    printf("\n\n");
    printf("Memory allocation for 2D integer array\n\n");
    ptr_iArray_rc = (int **)malloc(sizeof(int *) * num_rows_rc);
    if (ptr_iArray_rc == NULL)
    {
        printf("Failed to allocate memory to %d rows integer array! Exiting now..", num_rows_rc);
        exit(0);
    }
    else
    {
        printf("Memory allocation to %d rows of 2D integer array succeeded!!\n\n", num_rows_rc);
    }
    
    // allocating memory to each row which is a 1D array containing consisting of columns which contain the actual integers
    for (i_rc = 0; i_rc < num_rows_rc; i_rc++)
    {
        ptr_iArray_rc[i_rc] = (int *)malloc(sizeof(int) * num_cols_rc);
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

    // FILLING UP VALUES
    for (i_rc = 0; i_rc < num_rows_rc; i_rc++)
    {
        for (j_rc = 0; j_rc < num_cols_rc; j_rc++)
        {
            ptr_iArray_rc[i_rc][j_rc] = (i_rc + 1) * (j_rc + 1);
        }
    }

    // DISPLAYING VALUES
    for (i_rc = 0; i_rc < num_rows_rc; i_rc++)
    {
        printf("Base address of row %d: ptr_iArray[%d] = %p \t At address %p\n", i_rc, j_rc, ptr_iArray_rc[i_rc], &ptr_iArray_rc[i_rc]);
    }
    
    printf("\n\n");

    for (i_rc = 0; i_rc < num_rows_rc; i_rc++)
    {
        for (j_rc = 0; j_rc < num_cols_rc; j_rc++)
        {
            printf("ptr_iArray[%d][%d] = %d \t at address: %p\n", i_rc, j_rc, ptr_iArray_rc[i_rc][j_rc], &ptr_iArray_rc[i_rc][j_rc]);
        }
        printf("\n");
    }

    // freeing memory allocated to each row
    for (i_rc = (num_rows_rc - 1); i_rc >= 0; i_rc--)
    {
        if (ptr_iArray_rc[i_rc])
        {
            free(ptr_iArray_rc[i_rc]);
            ptr_iArray_rc[i_rc] = NULL;
            printf("MEMORY ALLOCATED FOR ROW %d OF 2D INTEGER ARRAY HAS BEEN SUCCESSFULLY FREED!\n\n", i_rc);
        }
    }

    // freeing memory allocated to 1D array consisting of base addreeses of rows
    if (ptr_iArray_rc)
    {
        free(ptr_iArray_rc);
        ptr_iArray_rc = NULL;
        printf("Memory allocated to ptr_iArray has been successfully freed!!\n\n");
    }

    return(0);
}
