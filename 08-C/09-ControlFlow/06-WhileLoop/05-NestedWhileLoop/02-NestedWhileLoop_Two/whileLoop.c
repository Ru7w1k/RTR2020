#include <stdio.h>

int main(void)
{
    // variable declarations
    int rmc_i, rmc_j, rmc_k;

    // code
    printf("\n\n");

    rmc_i = 1;
    while (rmc_i <= 10)
    {
        printf("i = %d\n", rmc_i);
        printf("--------\n\n");

        rmc_j = 1;
        while (rmc_j <= 5)
        {
            printf("\tj = %d\n", rmc_j);
            printf("\t--------\n\n");

            rmc_k = 1;
            while (rmc_k <= 3)
            {
                printf("\t\tk = %d\n", rmc_k);
                printf("\t\t--------\n\n");

                rmc_k++;
            }

            rmc_j++;
        }

        rmc_i++;
        printf("\n\n");
    }
    return (0);
}
