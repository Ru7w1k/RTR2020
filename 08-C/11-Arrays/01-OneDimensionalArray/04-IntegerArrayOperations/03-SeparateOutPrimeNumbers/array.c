#include <stdio.h>

#define NUM_ELEMENTS 10

int main(void)
{
    // variable declarations
    int iArray_rmc[NUM_ELEMENTS];
    int rmc_i, rmc_num, rmc_j, rmc_count = 0;

    // code
    printf("\n\n");
    printf("Enter elements for array: \n");

    // array element input
    for (rmc_i = 0; rmc_i < NUM_ELEMENTS; rmc_i++)
    {
        scanf("%d", &rmc_num);

        // if 'rmc_num' is negative ( < 0), convert it to positive (multiply by -1)
        if (rmc_num < 0)
            rmc_num = -1 * rmc_num;

        iArray_rmc[rmc_i] = rmc_num;
    }

    // printing entire array
    printf("\n\n");
    printf("Array elements are: \n\n");
    for (rmc_i = 0; rmc_i < NUM_ELEMENTS; rmc_i++)
    {
        printf("%d\n", iArray_rmc[rmc_i]);
    }

    // separate out prime numbers from array
    printf("\n\n");
    printf("Prime numbers amongst the array are: \n\n");
    for (rmc_i = 0; rmc_i < NUM_ELEMENTS; rmc_i++)
    {
        for (rmc_j = 1; rmc_j <= iArray_rmc[rmc_i]; rmc_j++)
        {
            if ((iArray_rmc[rmc_i] % rmc_j) == 0)
                rmc_count++;
        }
        // NUMBER 1 IS NEITHER A PRIME NUMBER OR A COMPOSITE
        // IF A NUMBER IS PRIME, IT IS ONLY DIVISIBLE BY 1 AND ITSELF.
        // HENCE, IF A NUMBER IS PRIME, THE VALUE OF count WILL BE EXACTLY 2.
        // IF THE VALUE OF count IS GREATER THAN 2, THE NUMBER IS NOT PRIME.
        // THE VALUE OF COUNT WILL BE 1 ONLY IF iArray[i] IS 1.

        if (rmc_count == 2)
            printf("%d\n", iArray_rmc[rmc_i]);

        rmc_count = 0; // RESET count TO 0 FOR CHECKING THE NEXT NUMBER
    }

    return (0);
}
