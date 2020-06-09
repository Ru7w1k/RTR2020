#include <stdio.h>

#define NUM_ELEMENTS 10

int main(void)
{
    // variable declarations
    int iArray_rmc[NUM_ELEMENTS];
    int rmc_i, rmc_num;

    // code
    printf("\n\n");
    printf("Enter elements for array: \n");

    // array element input
    for (rmc_i = 0; rmc_i < NUM_ELEMENTS; rmc_i++)
    {
        scanf("%d", &rmc_num);
        iArray_rmc[rmc_i] = rmc_num;
    }

    // separate out even numbers from array
    printf("\n\n");
    printf("Even numbers amongst the array are: \n\n");
    for (rmc_i = 0; rmc_i < NUM_ELEMENTS; rmc_i++)
    {
        if ((iArray_rmc[rmc_i] % 2) == 0)
            printf("%d\n", iArray_rmc[rmc_i]);
    }

    // separate out odd numbers from array
    printf("\n\n");
    printf("Odd numbers amongst the array are: \n\n");
    for (rmc_i = 0; rmc_i < NUM_ELEMENTS; rmc_i++)
    {
        if ((iArray_rmc[rmc_i] % 2) != 0)
            printf("%d\n", iArray_rmc[rmc_i]);

    }



    return(0);
}


