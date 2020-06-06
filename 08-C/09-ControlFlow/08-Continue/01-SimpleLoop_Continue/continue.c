#include <stdio.h>

int main(void)
{
    // variable declarations
    int rmc_i;

    // code
    printf("\n\n");

    printf("Printing Even numbers from 0 to 100: \n\n");

    for (rmc_i = 0; rmc_i < 100; rmc_i++)
    {
        // condition for a number to be even number => division of number by 2 leaves no remainder
        // if remainder is not zero, then number is odd
        if (rmc_i % 2 != 0)
        {
            continue;
        }
        else
        {
            printf("\t%d\n", rmc_i);
        }
    }

    return (0);
}
