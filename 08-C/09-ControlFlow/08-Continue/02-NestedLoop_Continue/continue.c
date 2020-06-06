#include <stdio.h>

int main(void)
{
    // variable declarations
    int rmc_i, rmc_j;

    // code
    printf("\n\n");

    printf("Outer loop prints odd numbers between 1 and 10. \n\n");
    printf("Inner loop prints even numbers between 1 and 10 for every odd number printed by outer loop. \n\n");

    // condition for a number to be even number => division of number by 2 leaves no remainder
    // if remainder is not zero, then number is odd

    for (rmc_i = 1; rmc_i <= 10; rmc_i++)
    {
        if (rmc_i % 2 != 0) // i is ODD
        {
            printf("i = %d\n", rmc_i);
            printf("---------\n");
            for (rmc_j = 1; rmc_j <= 10; rmc_j++)
            {
                if (rmc_j % 2 == 0) // i is EVEN
                {
                    printf("\tj = %d\n", rmc_j);
                }
                else // i is ODD
                {
                    continue;
                }
            }
            printf("\n\n");
        }
        else // i is EVEN
        {
            continue;
        }
    }

    printf("\n\n");

    return (0);
}
