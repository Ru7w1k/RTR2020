#include <stdio.h>

int main(void)
{
    // variable declarations
    int iArray_rmc[3][5]; // 3 ROWS (0, 1, 2) AND 5 COLUMNS (0, 1, 2, 3, 4)
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

    // PIECE-MEAL ASSIGNMENT
    // ROW 1
    iArray_rmc[0][0] = 21;
    iArray_rmc[0][1] = 42;
    iArray_rmc[0][2] = 63;
    iArray_rmc[0][3] = 84;
    iArray_rmc[0][4] = 105;

    // ROW 2
    iArray_rmc[1][0] = 31;
    iArray_rmc[1][1] = 52;
    iArray_rmc[1][2] = 73;
    iArray_rmc[1][3] = 94;
    iArray_rmc[1][4] = 115;


    // ROW 3
    iArray_rmc[2][0] = 11;
    iArray_rmc[2][1] = 32;
    iArray_rmc[2][2] = 53;
    iArray_rmc[2][3] = 74;
    iArray_rmc[2][4] = 95;



    // DISPLAY
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


