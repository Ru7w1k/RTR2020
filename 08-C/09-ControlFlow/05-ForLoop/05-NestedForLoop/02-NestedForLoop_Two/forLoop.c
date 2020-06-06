#include <stdio.h>

int main(void)
{
    // variable declarations
    int rmc_i, rmc_j, rmc_k;

    // code
    printf("\n\n");

    for (rmc_i = 1; rmc_i <= 10; rmc_i++)
    {
        printf("i = %d\n", rmc_i);
        printf("--------\n\n");
        for (rmc_j = 1; rmc_j <= 5; rmc_j++)
        {
            printf("\tj = %d\n", rmc_j);
            printf("\t--------\n\n");
            for (rmc_k = 1; rmc_k <= 3; rmc_k++)
            {
                printf("\t\tk = %d\n", rmc_k);
            }
            printf("\n\n");
        }
        printf("\n\n");
    }
    return (0);
}
