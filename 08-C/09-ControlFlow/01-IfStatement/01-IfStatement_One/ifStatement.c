#include <stdio.h>

int main(void)
{
    // variable declarations
    int rmc_a, rmc_b, rmc_p;

    // code
    rmc_a = 11;
    rmc_b = 35;
    rmc_p= 35;

    printf("\n\n");

    if (rmc_a < rmc_b)
    {
        printf("A is less than B!!\n\n");
    }

    if (rmc_b != rmc_p)
    {
        printf("B is not equal to P!!\n\n");
    }

    printf("Both comparisions have been done!!\n\n");
    
    return(0);
}
