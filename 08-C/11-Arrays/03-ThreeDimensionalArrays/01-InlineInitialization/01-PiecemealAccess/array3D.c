#include <stdio.h>

int main(void)
{
    // variable declarations
    int iArray_rmc[5][3][2] = {
        { { 10, 20 }, { 30, 40 }, { 50, 60 } },
        { {  8, 16 }, { 24, 32 }, { 40, 48 } },
        { {  6, 12 }, { 18, 24 }, { 30, 36 } },
        { {  4,  8 }, { 12, 16 }, { 20, 24 } },
        { {  2,  4 }, {  6,  8 }, { 10, 12 } }
    };
    int rmc_int_size;
    int rmc_iArray_size;
    int rmc_iArray_num_elements, rmc_iArray_num_width, rmc_iArray_num_height, rmc_iArray_depth;

    // code
    printf("\n\n");

    rmc_int_size = sizeof(int);

    rmc_iArray_size = sizeof(iArray_rmc);
    printf("Size of Three Dimensional (3D) Integer array is = %d\n\n", rmc_iArray_size);

    rmc_iArray_num_width = rmc_iArray_size / sizeof(iArray_rmc[0]);
    printf("Number of Rows (width) in Three Dimensional (3D) Integer array is = %d\n\n", rmc_iArray_num_width);

    rmc_iArray_num_height = sizeof(iArray_rmc[0]) / sizeof(iArray_rmc[0][0]);
    printf("Number of Columns (Height) in Three Dimensional (3D) Integer array is = %d\n\n", rmc_iArray_num_height);

    rmc_iArray_depth = sizeof(iArray_rmc[0][0]) / rmc_int_size;
    printf("Number of Columns (Height) in Three Dimensional (3D) Integer array is = %d\n\n", rmc_iArray_depth);

    rmc_iArray_num_elements = rmc_iArray_num_width * rmc_iArray_num_height * rmc_iArray_depth;
    printf("Number of Elements in Three Dimensional (3D) Integer array is = %d\n\n", rmc_iArray_num_elements);

    printf("\n\n");
    printf("Elements in Integer 3D array: \n\n");

    // PIECE-MEAL DISPLAY
    // ROW 1
    printf("******** ROW 1 *********\n");
    printf("******* COLUMN 1 *******\n");
    printf("iArray[0][0][0] = %d\n", iArray_rmc[0][0][0]);
    printf("iArray[0][0][1] = %d\n", iArray_rmc[0][0][1]);
    printf("\n");

    printf("******* COLUMN 2 *******\n");
    printf("iArray[0][1][0] = %d\n", iArray_rmc[0][1][0]);
    printf("iArray[0][1][1] = %d\n", iArray_rmc[0][1][1]);
    printf("\n");

    printf("******* COLUMN 3 *******\n");
    printf("iArray[0][2][0] = %d\n", iArray_rmc[0][2][0]);
    printf("iArray[0][2][1] = %d\n", iArray_rmc[0][2][1]);
    printf("\n");

    // ROW 2
    printf("******** ROW 2 *********\n");
    printf("******* COLUMN 1 *******\n");
    printf("iArray[1][0][0] = %d\n", iArray_rmc[1][0][0]);
    printf("iArray[1][0][1] = %d\n", iArray_rmc[1][0][1]);
    printf("\n");

    printf("******* COLUMN 2 *******\n");
    printf("iArray[1][1][0] = %d\n", iArray_rmc[1][1][0]);
    printf("iArray[1][1][1] = %d\n", iArray_rmc[1][1][1]);
    printf("\n");

    printf("******* COLUMN 3 *******\n");
    printf("iArray[1][2][0] = %d\n", iArray_rmc[1][2][0]);
    printf("iArray[1][2][1] = %d\n", iArray_rmc[1][2][1]);
    printf("\n");

    // ROW 3
    printf("******** ROW 3 *********\n");
    printf("******* COLUMN 1 *******\n");
    printf("iArray[2][0][0] = %d\n", iArray_rmc[2][0][0]);
    printf("iArray[2][0][1] = %d\n", iArray_rmc[2][0][1]);
    printf("\n");

    printf("******* COLUMN 2 *******\n");
    printf("iArray[2][1][0] = %d\n", iArray_rmc[2][1][0]);
    printf("iArray[2][1][1] = %d\n", iArray_rmc[2][1][1]);
    printf("\n");

    printf("******* COLUMN 3 *******\n");
    printf("iArray[2][2][0] = %d\n", iArray_rmc[2][2][0]);
    printf("iArray[2][2][1] = %d\n", iArray_rmc[2][2][1]);
    printf("\n");

    // ROW 4
    printf("******** ROW 4 *********\n");
    printf("******* COLUMN 1 *******\n");
    printf("iArray[3][0][0] = %d\n", iArray_rmc[3][0][0]);
    printf("iArray[3][0][1] = %d\n", iArray_rmc[3][0][1]);
    printf("\n");

    printf("******* COLUMN 2 *******\n");
    printf("iArray[3][1][0] = %d\n", iArray_rmc[3][1][0]);
    printf("iArray[3][1][1] = %d\n", iArray_rmc[3][1][1]);
    printf("\n");

    printf("******* COLUMN 3 *******\n");
    printf("iArray[3][2][0] = %d\n", iArray_rmc[3][2][0]);
    printf("iArray[3][2][1] = %d\n", iArray_rmc[3][2][1]);
    printf("\n");

    // ROW 5
    printf("******** ROW 5 *********\n");
    printf("******* COLUMN 1 *******\n");
    printf("iArray[4][0][0] = %d\n", iArray_rmc[4][0][0]);
    printf("iArray[4][0][1] = %d\n", iArray_rmc[4][0][1]);
    printf("\n");

    printf("******* COLUMN 2 *******\n");
    printf("iArray[4][1][0] = %d\n", iArray_rmc[4][1][0]);
    printf("iArray[4][1][1] = %d\n", iArray_rmc[4][1][1]);
    printf("\n");

    printf("******* COLUMN 3 *******\n");
    printf("iArray[4][2][0] = %d\n", iArray_rmc[4][2][0]);
    printf("iArray[4][2][1] = %d\n", iArray_rmc[4][2][1]);
    printf("\n");


  
    return(0);
}


