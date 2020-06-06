#include <stdio.h>

int main(void)
{
    // variable declarations
    int rmc_i_num, rmc_num, rmc_i;

    // code
    printf("\n\n");
    printf("Enter an integer value from which iteration must begin: ");
    scanf("%d", &rmc_i_num);

    printf("How many digits do you want to print from %d onwards: ", rmc_i_num);
    scanf("%d", &rmc_num);

    printf("Printing digits %d to %d: \n\n", rmc_i_num, (rmc_i_num+rmc_num));

    rmc_i = rmc_i_num;
    while (rmc_i <= (rmc_i_num + rmc_num))
    {
        printf("\t%d\n", rmc_i);
        rmc_i++;
    }

    printf("\n\n");

    return(0);
}
