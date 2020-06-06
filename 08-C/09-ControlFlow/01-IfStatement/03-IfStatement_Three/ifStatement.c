#include <stdio.h>

int main(void)
{
    // variable declarations
    int rmc_num;

    // code
    printf("\n\n");

    printf("Enter value for num: ");
    scanf("%d", &rmc_num);

    if (rmc_num < 0)
    {
        printf("Num = %d is less that 0 (negative).\n\n", rmc_num);
    }

    if ((rmc_num > 0) && (rmc_num <= 100))
    {
        printf("Num = %d is between 0 and 100.\n\n", rmc_num);
    }

    if ((rmc_num > 100) && (rmc_num <= 200))
    {
        printf("Num = %d is between 100 and 200.\n\n", rmc_num);
    }

    if ((rmc_num > 200) && (rmc_num <= 300))
    {
        printf("Num = %d is between 200 and 300.\n\n", rmc_num);
    }

    if ((rmc_num > 300) && (rmc_num <= 400))
    {
        printf("Num = %d is between 300 and 400.\n\n", rmc_num);
    }

    if ((rmc_num > 400) && (rmc_num <= 500))
    {
        printf("Num = %d is between 400 and 500.\n\n", rmc_num);
    }

    if (rmc_num > 500)
    {
        printf("Num = %d is greater that 500.\n\
            n", rmc_num);
    }


    return(0);
}
