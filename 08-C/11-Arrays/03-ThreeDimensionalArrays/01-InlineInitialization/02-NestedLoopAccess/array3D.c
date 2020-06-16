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
    int rmc_i, rmc_j, rmc_k;

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

    for (rmc_i = 0; rmc_i < rmc_iArray_num_width; rmc_i++)
    {
        printf("******** ROW %d *********\n", rmc_i + 1);
        for (rmc_j = 0; rmc_j < rmc_iArray_num_height; rmc_j++)
        {
            printf("******* COLUMN %d *******\n", rmc_j + 1);
            for (rmc_k = 0; rmc_k < rmc_iArray_depth; rmc_k++)
            {
                printf("iArray[%d][%d][%d] = %d\n", rmc_i, rmc_j, rmc_k, iArray_rmc[rmc_i][rmc_j][rmc_k]);
            }
            printf("\n");
        }
        printf("\n\n");
    }
  
    return(0);
}


