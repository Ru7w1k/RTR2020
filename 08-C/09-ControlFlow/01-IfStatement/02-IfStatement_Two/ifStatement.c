#include <stdio.h>

int main(void)
{
    // variable declarations
    int rmc_age;

    // code
    printf("\n\n");
    printf("Enter Age: ");
    scanf("%d", &rmc_age);

    if (rmc_age >= 18)
    {
        printf("You are eligible for voting!!\n");
    }

    return(0);
}
