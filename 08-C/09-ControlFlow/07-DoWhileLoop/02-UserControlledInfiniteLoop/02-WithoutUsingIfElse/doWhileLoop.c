#include <stdio.h>

int main(void)
{
    // variable declarations
    char rmc_option, rmc_ch = '\0';

    // code
    printf("\n\n");
    printf("Once the infinite loop begins, enter 'Q' or 'q' to quit the loop: \n\n");

    do
    {
        do
        {
            printf("\n");
            printf("In loop...\n");
            rmc_ch = getch();                     // control flow waits for character input
        } while (rmc_ch != 'Q' && rmc_ch != 'q'); // infinite loop

        printf("\n\n");
        printf("Exiting user controlled infinite loop..");
        printf("\n\n");

        printf("Do you want to begin the user controlled infinite loop again? (Y/y for yes, any other for no): \n\n");
        rmc_option = getch();
    } while(rmc_option == 'Y' || rmc_option == 'y');

    return (0);
}
