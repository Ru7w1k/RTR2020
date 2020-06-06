#include <stdio.h>

int main(void)
{
    // variable declarations
    char rmc_option, rmc_ch = '\0';

    // code
    printf("\n\n");
    printf("Once the infinite loop begins, enter 'Q' or 'q' to quit the loop: \n\n");
    printf("Enter 'Y' or 'y' to initiate user controlled infinite loop: ");
    printf("\n\n");
    
    rmc_option = getch();
    if (rmc_option == 'Y' || rmc_option == 'y')
    {
        while(1) // infinite loop
        {
            printf("In loop...\n");
            rmc_ch = getch();
            if (rmc_ch == 'Q' || rmc_ch == 'q')
                break; //  user controlled exiting of infinite loop
        }
    }
    else
    {
        printf("You must press 'Y' or 'y' to initiate user controlled infinite loop..Please try again..\n\n");
    }
    

    printf("\n\n");
    printf("Exiting user controlled infinite loop..");
    printf("\n\n");

    return(0);
}
