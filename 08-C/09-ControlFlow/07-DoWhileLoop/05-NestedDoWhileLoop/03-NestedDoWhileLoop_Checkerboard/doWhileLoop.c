#include <stdio.h>

int main(void)
{
    // variable declarations
    int rmc_i, rmc_j, rmc_c;

    // code
    printf("\n\n");

    rmc_i = 0;
    do
    {
        rmc_j = 0;
        do
        {
            rmc_c = ((rmc_i & 0x8) == 0) ^ ((rmc_j & 0x8) == 0);
            if (rmc_c == 0)
                printf("  ");

            if (rmc_c == 1)
                printf("* ");

            rmc_j++;
        } while (rmc_j < 64);
        printf("\n");
        rmc_i++;
    } while (rmc_i < 64);

    return (0);
}
