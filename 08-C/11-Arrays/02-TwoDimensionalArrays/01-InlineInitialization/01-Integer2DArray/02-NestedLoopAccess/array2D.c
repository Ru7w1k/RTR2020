#include <stdio.h>

int main(void)
{
    // variable declarations
    int iArray_rmc[5][3] = { {1, 2, 3}, {2, 4, 6}, {3, 6, 9}, {4, 8, 12}, {5, 10, 15} }; // IN-LINE INITIALIZATION
    int rmc_int_size;
    int rmc_iArray_size;
    int rmc_iArray_num_elements, rmc_iArray_num_rows, rmc_iArray_num_columns;
    int rmc_i, rmc_j;

    // code
    printf("\n\n");

    rmc_int_size = sizeof(int);

    rmc_iArray_size = sizeof(iArray_rmc);
    printf("Size of Two Dimensional (2D) Integer array is = %d\n\n", rmc_iArray_size);

    rmc_iArray_num_rows = rmc_iArray_size / sizeof(iArray_rmc[0]);
    printf("Number of Rows in Two Dimensional (2D) Integer array is = %d\n\n", rmc_iArray_num_rows);

    rmc_iArray_num_columns = sizeof(iArray_rmc[0]) / rmc_int_size;
    printf("Number of Columns in Two Dimensional (2D) Integer array is = %d\n\n", rmc_iArray_num_columns);

    rmc_iArray_num_elements = rmc_iArray_num_rows * rmc_iArray_num_columns;
    printf("Number of Elements in Two Dimensional (2D) Integer array is = %d\n\n", rmc_iArray_num_elements);

    printf("\n\n");
    printf("Elements in 2D array: \n\n");

    // ARRAY INDICES BEGIN FROM 0, HENCE, 1ST ROW IS ACTUALLY 0TH ROW AND 1ST COLUMN IS ACTUALLY 0TH COLUMN

    for (rmc_i = 0; rmc_i < rmc_iArray_num_rows; rmc_i++)
    {
        printf("****** ROW %d ******\n", (rmc_i + 1));
        for (rmc_j = 0; rmc_j < rmc_iArray_num_columns; rmc_j++)
        {
            printf("iArray[%d][%d] = %d\n", rmc_i, rmc_j, iArray_rmc[rmc_i][rmc_j]);
        }
        printf("\n\n");
    }

    return(0);
}


