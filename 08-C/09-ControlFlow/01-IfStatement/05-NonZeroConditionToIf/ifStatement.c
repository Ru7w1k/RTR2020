#include <stdio.h>

int main(void)
{
    // variable declarations
    int rmc_a;

    // code
    printf("\n\n");

    rmc_a = 6;
    if (rmc_a)
    {
        printf("if block 1: 'A' exists and has value = %d !!\n\n", rmc_a);
    }

    rmc_a = -6;
    if (rmc_a)
    {
        printf("if block 2: 'A' exists and has value = %d !!\n\n", rmc_a);
    }

    rmc_a = 0;
    if (rmc_a)
    {
        printf("if block 3: 'A' exists and has value = %d !!\n\n", rmc_a);
    }

    printf("All three if statements are done!!\n\n");
    
    return(0);
}
