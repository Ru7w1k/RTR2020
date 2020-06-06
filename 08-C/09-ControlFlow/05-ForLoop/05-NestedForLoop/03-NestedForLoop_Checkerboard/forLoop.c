#include <stdio.h>

int main(void)
{
    // variable declarations
    int rmc_i, rmc_j, rmc_c;

    // code
    printf("\n\n");

    for (rmc_i = 0; rmc_i < 64; rmc_i++)
    {
        for (rmc_j = 0; rmc_j < 64; rmc_j++)
        {
            rmc_c = ((rmc_i & 0x8) == 0) ^ ((rmc_j & 0x8) == 0);
            if (rmc_c == 0)
                printf("  ");

            if (rmc_c == 1)
                printf("* ");
        }
        printf("\n");
    }
    return (0);
}
