#include <stdio.h>

#define NUM_ROWS 5
#define NUM_COLS 3
#define DEPTH 2

int main(void)
{
    // variable declarations
    int iArray3D_rmc[NUM_ROWS][NUM_COLS][DEPTH] = {
        { { 10, 20 }, { 30, 40 }, { 50, 60 } },
        { {  8, 16 }, { 24, 32 }, { 40, 48 } },
        { {  6, 12 }, { 18, 24 }, { 30, 36 } },
        { {  4,  8 }, { 12, 16 }, { 20, 24 } },
        { {  2,  4 }, {  6,  8 }, { 10, 12 } }
    };
    
    int iArray1D_rmc[NUM_ROWS * NUM_COLS * DEPTH];
    int rmc_i, rmc_j, rmc_k;

    // code
  
    // display 3D array
    printf("\n\n");
    printf("Elements in Integer 3D array: \n\n");

    for (rmc_i = 0; rmc_i < NUM_ROWS; rmc_i++)
    {
        printf("******** ROW %d *********\n", rmc_i + 1);
        for (rmc_j = 0; rmc_j < NUM_COLS; rmc_j++)
        {
            printf("******* COLUMN %d *******\n", rmc_j + 1);
            for (rmc_k = 0; rmc_k < DEPTH; rmc_k++)
            {
                printf("iArray3D[%d][%d][%d] = %d\n", rmc_i, rmc_j, rmc_k, iArray3D_rmc[rmc_i][rmc_j][rmc_k]);
            }
            printf("\n");
        }
        printf("\n\n");
    }

    // converting 3D to 1D
    for (rmc_i = 0; rmc_i < NUM_ROWS; rmc_i++)
    {
        for (rmc_j = 0; rmc_j < NUM_COLS; rmc_j++)
        {
            for (rmc_k = 0; rmc_k < DEPTH; rmc_k++)
            {
                iArray1D_rmc[(rmc_i * NUM_COLS * DEPTH) + (rmc_j * DEPTH) + rmc_k] = iArray3D_rmc[rmc_i][rmc_j][rmc_k];
            }
        }
    }

    // displaying 1D array
    printf("\n\n");
    printf("Elements in Integer 1D array: \n\n");
    for (rmc_i = 0; rmc_i < NUM_ROWS * NUM_COLS * DEPTH; rmc_i++)
    {
        printf("iArray1D[%d] = %d\n", rmc_i, iArray1D_rmc[rmc_i]);
    }
  
    return(0);
}


