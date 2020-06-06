#include <stdio.h>

int main(void)
{
    // variable declarations
    int rmc_i, rmc_j;

    // code
    printf("\n\n");

    rmc_i = 1;
    do
    {
        printf("i = %d\n", rmc_i);
        printf("--------\n\n");

        rmc_j = 1;
        do
        {
            printf("\tj = %d\n", rmc_j);
            rmc_j++;
        } while (rmc_j <= 5);

        rmc_i++;
        printf("\n\n");
    } while (rmc_i <= 10);
    return (0);
}
