#include <stdio.h>
#include <conio.h>

int main(void)
{
    // variable declarations
    int rmc_i;
    char rmc_ch;

    // code
    printf("\n\n");

    printf("Printing even numbers from 1 to 100 for every user input. Exiting loop when user inputs 'Q' or 'q' \n\n");
    printf("Enter 'Q' or 'q' to exit loop: \n\n");

    for (rmc_i = 1; rmc_i <= 100; rmc_i++)
    {
        printf("\t%d\n", rmc_i);
        rmc_ch = getch();
        if (rmc_ch == 'q' || rmc_ch == 'Q')
        {
            break;
        }
    }

    printf("\n\n");
    printf("Exiting loop...");
    printf("\n\n");

    return (0);
}
