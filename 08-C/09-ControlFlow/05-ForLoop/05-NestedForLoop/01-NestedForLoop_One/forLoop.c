#include <stdio.h>

int main(void)
{
    // variable declarations
    int rmc_i, rmc_j;

    // code
    printf("\n\n");
    
    for (rmc_i = 1; rmc_i <= 10; rmc_i++)
    {
        printf("i = %d\n", rmc_i);
        printf("--------\n\n");
        for(rmc_j = 1; rmc_j <= 5; rmc_j++)
        {
            printf("\tj = %d\n", rmc_j);
        }
        printf("\n\n");
    }
    return(0);
}
