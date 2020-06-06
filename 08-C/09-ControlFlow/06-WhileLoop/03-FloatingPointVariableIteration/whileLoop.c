#include <stdio.h>

int main(void)
{
    // variable declarations
    float rmc_f;
    float rmc_f_num = 2.5f; // simply change this value ONLY to get different outputs

    // code
    printf("\n\n");
    printf("Printing numbers %f to %f: \n\n", rmc_f_num, (rmc_f_num*10.0f));

    rmc_f = rmc_f_num;
    while ( rmc_f <= (rmc_f_num * 10.0f))
    {
        printf("\t%f\n", rmc_f);
        rmc_f = rmc_f + rmc_f_num;
    }

    printf("\n\n");

    return(0);
}
