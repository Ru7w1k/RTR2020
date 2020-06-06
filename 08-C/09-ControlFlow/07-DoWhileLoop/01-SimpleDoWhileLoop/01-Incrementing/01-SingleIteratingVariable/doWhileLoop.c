#include <stdio.h>

int main(void)
{
    // variable declarations
    int rmc_i;

    // code
    printf("\n\n");

    printf("Printing digits 1 to 10: \n\n");

    rmc_i = 1;
    do
    {
        printf("\t%d\n", rmc_i);
        rmc_i++;
    } while (rmc_i <= 10);

    printf("\n\n");

    return (0);
}
