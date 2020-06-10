#include <stdio.h>

int main(void)
{
    // variable declarations
    int iArray_rmc[5][3] = { {1, 2, 3}, {2, 4, 6}, {3, 6, 9}, {4, 8, 12}, {5, 10, 15} }; // IN-LINE INITIALIZATION
    int rmc_int_size;
    int rmc_iArray_size;
    int rmc_iArray_num_elements, rmc_iArray_num_rows, rmc_iArray_num_columns;

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

    // ROW 1
    printf("****** ROW 1 ******\n");
    printf("iArray[0][0] = %d\n", iArray_rmc[0][0]); // COLUMN 1 (0th element)
    printf("iArray[0][1] = %d\n", iArray_rmc[0][1]); // COLUMN 2 (1st element)
    printf("iArray[0][2] = %d\n", iArray_rmc[0][2]); // COLUMN 3 (3rd element)

    printf("\n\n");

    // ROW 2
    printf("****** ROW 2 ******\n");
    printf("iArray[1][0] = %d\n", iArray_rmc[1][0]); // COLUMN 1 (0th element)
    printf("iArray[1][1] = %d\n", iArray_rmc[1][1]); // COLUMN 2 (1st element)
    printf("iArray[1][2] = %d\n", iArray_rmc[1][2]); // COLUMN 3 (3rd element)

    printf("\n\n");

    // ROW 3
    printf("****** ROW 3 ******\n");
    printf("iArray[2][0] = %d\n", iArray_rmc[2][0]); // COLUMN 1 (0th element)
    printf("iArray[2][1] = %d\n", iArray_rmc[2][1]); // COLUMN 2 (1st element)
    printf("iArray[2][2] = %d\n", iArray_rmc[2][2]); // COLUMN 3 (3rd element)

    printf("\n\n");

    // ROW 4
    printf("****** ROW 4 ******\n");
    printf("iArray[3][0] = %d\n", iArray_rmc[3][0]); // COLUMN 1 (0th element)
    printf("iArray[3][1] = %d\n", iArray_rmc[3][1]); // COLUMN 2 (1st element)
    printf("iArray[3][2] = %d\n", iArray_rmc[3][2]); // COLUMN 3 (3rd element)

    printf("\n\n");

    // ROW 5
    printf("****** ROW 5 ******\n");
    printf("iArray[4][0] = %d\n", iArray_rmc[4][0]); // COLUMN 1 (0th element)
    printf("iArray[4][1] = %d\n", iArray_rmc[4][1]); // COLUMN 2 (1st element)
    printf("iArray[4][2] = %d\n", iArray_rmc[4][2]); // COLUMN 3 (3rd element)

    printf("\n\n");

    return(0);
}


