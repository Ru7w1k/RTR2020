#include <stdio.h>

#define NUM_ROWS 5
#define NUM_COLS 3

int main(void)
{
    // variable declarations
    int iArray_2D_rmc[NUM_ROWS][NUM_COLS]; // TOTAL NUMBER OF ELEMENTS = NUM_ROWS * NUM_COLS
    int iArray_1D_rmc[NUM_ROWS * NUM_COLS]; 

    int rmc_i, rmc_j;
    int rmc_num;

    // code
    printf("Enter elements of your choice to fill up the interger 2D array: \n\n");
    for (rmc_i = 0; rmc_i < NUM_ROWS; rmc_i++)
    {
        printf("FOR ROW NUMBER %d: \n", (rmc_i + 1));
        for (rmc_j = 0; rmc_j < NUM_COLS; rmc_j++)
        {
            printf("Enter Element Number %d: \n", (rmc_j + 1));
            scanf("%d", &rmc_num);
            iArray_2D_rmc[rmc_i][rmc_j] = rmc_num;
        }
        printf("\n\n");
    }

    // DISPLAY OF 2D ARRAY
    for (rmc_i = 0; rmc_i < NUM_ROWS; rmc_i++)
    {
        printf("****** ROW %d ******\n", (rmc_i + 1));
        for (rmc_j = 0; rmc_j < NUM_COLS; rmc_j++)
        {
            printf("iArray_2D[%d][%d] = %d\n", rmc_i, rmc_j, iArray_2D_rmc[rmc_i][rmc_j]);
        }
        printf("\n\n");
    }

    // converting 2D integer array to 1D integer array
    for (rmc_i = 0; rmc_i < NUM_ROWS; rmc_i++)
    {
        for (rmc_j = 0; rmc_j < NUM_COLS; rmc_j++)
        {
            iArray_1D_rmc[(rmc_i * NUM_COLS) + rmc_j] = iArray_2D_rmc[rmc_i][rmc_j];
        }
    }

    // PRINTING 1D ARRAY
    printf("\n\n");
    printf("One-Dimensional (1D) Array of Integers: \n\n");
    for (rmc_i = 0; rmc_i < (NUM_ROWS * NUM_COLS); rmc_i++)
    {
        printf("iArray_1D[%d] = %d\n", rmc_i, iArray_1D_rmc[rmc_i]);
    }

    return(0);
}


