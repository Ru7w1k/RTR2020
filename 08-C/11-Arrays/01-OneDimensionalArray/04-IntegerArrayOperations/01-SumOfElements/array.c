#include <stdio.h>

#define NUM_ELEMENTS 10

int main(void)
{
    // variable declarations
    int iArray_rmc[NUM_ELEMENTS];
    int rmc_i, rmc_num, rmc_sum = 0;

    // code
    printf("\n\n");
    printf("Enter elements for 'Integer' array: \n");
    for (rmc_i = 0; rmc_i < NUM_ELEMENTS; rmc_i++)
    {
        scanf("%d", &rmc_num);
        iArray_rmc[rmc_i] = rmc_num;
    }

    for (rmc_i = 0; rmc_i < NUM_ELEMENTS; rmc_i++)
    {
        rmc_sum += iArray_rmc[rmc_i];
    }

    printf("\n\n");
    printf("Sum of ALL elements of array = %d \n\n", rmc_sum
    );

    return(0);
}


