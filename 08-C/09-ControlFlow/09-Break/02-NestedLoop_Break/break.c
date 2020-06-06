#include <stdio.h>
#include <conio.h>

int main(void)
{
    // variable declarations
    int rmc_i, rmc_j;

    // code
    printf("\n\n");

    for (rmc_i = 1; rmc_i <= 20; rmc_i++)
    {
        for (rmc_j = 1; rmc_j <= 20; rmc_j++)
        {
        if(rmc_j > rmc_i)
        {
            break;
        }
        else
        {
            printf("* ");
        }
        
        }
        printf("\n");
    }

    printf("\n\n");

    return (0);
}
