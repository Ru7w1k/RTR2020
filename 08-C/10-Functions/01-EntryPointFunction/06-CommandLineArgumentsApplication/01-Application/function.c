#include <stdio.h>  // contains declaration of printf()
#include <ctype.h>  // contains declaration of atoi()
#include <stdlib.h> // contains declaration of exit()

// entry-point function => main() => valid return type (int) and 2 parameters (int argc, char *argv[]), hence no command line arguments
int main(int argc, char *argv[], char *envp[])
{
    // variable declarations
    int rmc_i;
    int rmc_num;
    int rmc_sum = 0;

    // code
    if (argc == 1)
    {
        printf("\n\n");
        printf("No numbers given for addition! Exiting now..\n"); // library function
        printf("Usage: function <first number> <second number> ...\n\n");
        exit(0);
    }

    // this program adds all command line arguments given in integer form only and outputs the sum
    // due to use of atoi(), all command line arguments of type other that int are ignored!
    printf("\n\n");
    printf("Sum of all integer command line argument is: \n\n");
    for (rmc_i = 0; rmc_i < argc; rmc_i++)
    {
        rmc_num = atoi(argv[rmc_i]);
        rmc_sum += rmc_num;
    }
    printf("Sum = %d\n\n", rmc_sum);

    return (0);
}
