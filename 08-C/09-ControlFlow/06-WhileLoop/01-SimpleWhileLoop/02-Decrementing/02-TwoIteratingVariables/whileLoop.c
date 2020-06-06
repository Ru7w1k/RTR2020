#include <stdio.h>

int main(void)
{
    // variable declarations
    int rmc_i, rmc_j;

    // code
    printf("\n\n");
    
    printf("Printing digits 10 to 1 and 100 to 10: \n\n");

    rmc_i = 10;
    rmc_j = 100;
    while (rmc_i >= 1, rmc_j >= 10)
    {
        printf("\t %d \t %d \n", rmc_i, rmc_j);
        rmc_i--;
        rmc_j -= 10;
    }

    printf("\n\n");
    
    return(0);
}
