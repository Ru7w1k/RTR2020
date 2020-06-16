#include <stdio.h>

int main(void)
{
    // variable declarations
    int iArray_1_rmc[5];
    int iArray_2_rmc[5][3];
    int iArray_3_rmc[100][100][5];

    int rmc_num_rows_2D;
    int rmc_num_columns_2D;

    int rmc_num_rows_3D;
    int rmc_num_columns_3D;
    int rmc_depth_3D;

    // code
    printf("\n\n");
    printf("Size of 1D integer array iArray_1 = %lu \n\n", sizeof(iArray_1_rmc));
    printf("Number of elements in 1D integer array iArray_1 = %lu \n\n", sizeof(iArray_1_rmc) / sizeof(int));

    printf("\n\n");
    printf("Size of 2D integer array iArray_2 = %lu \n\n", sizeof(iArray_2_rmc));
    
    printf("Number of rows in 2D integer array iArray_2 = %lu \n", sizeof(iArray_2_rmc) / sizeof(iArray_2_rmc[0]));
    rmc_num_rows_2D = sizeof(iArray_2_rmc) / sizeof(iArray_2_rmc[0]);

    printf("Number of columns in 2D integer array iArray_2 = %lu \n\n", sizeof(iArray_2_rmc[0]) / sizeof(iArray_2_rmc[0][0]));
    rmc_num_columns_2D = sizeof(iArray_2_rmc[0]) / sizeof(iArray_2_rmc[0][0]);

    printf("Number of total elements in 2D integer array iArray_2 = %lu \n\n", rmc_num_rows_2D * rmc_num_columns_2D);
    printf("\n\n");

    printf("Size of 3D integer array iArray_3 = %lu \n\n", sizeof(iArray_3_rmc));
    
    printf("Number of rows in 3D integer array iArray_3 = %lu \n", sizeof(iArray_3_rmc) / sizeof(iArray_3_rmc[0]));
    rmc_num_rows_3D = sizeof(iArray_3_rmc) / sizeof(iArray_3_rmc[0]);

    printf("Number of columns in 3D integer array iArray_3 = %lu \n\n", sizeof(iArray_3_rmc[0]) / sizeof(iArray_3_rmc[0][0]));
    rmc_num_columns_3D = sizeof(iArray_3_rmc[0]) / sizeof(iArray_3_rmc[0][0]);

    printf("Number of depths in 3D integer array iArray_3 = %lu \n\n", sizeof(iArray_3_rmc[0][0]) / sizeof(iArray_3_rmc[0][0][0]));
    rmc_depth_3D = sizeof(iArray_3_rmc[0][0]) / sizeof(iArray_3_rmc[0][0][0]);

    printf("Number of total elements in 3D integer array iArray_3 = %lu \n\n", rmc_num_rows_3D * rmc_num_columns_3D * rmc_depth_3D);
    printf("\n\n");

    return(0);
}


