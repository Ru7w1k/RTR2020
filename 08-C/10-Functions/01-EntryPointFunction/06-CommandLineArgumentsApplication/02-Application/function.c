#include <stdio.h> // contains declaration of printf()
#include <stdlib.h> // contains declaration of exit()

// entry-point function => main() => valid return type (int) and 2 parameters (int argc, char *argv[]), hence no command line arguments
int main(int argc, char *argv[], char *envp[])
{
    // variable declarations
    int rmc_i;

    // code
    if (argc != 4) // program name + first name + middle name + surname = 4 command line args required
    {
        printf("\n\n");
        printf("Invalid usage! Exiting now..\n\n");
        printf("Usage: function <first name> <middle name> <last name>\n\n");
        exit(0);
    }

    // the program prints your full name as entered in the command line arguments
    printf("\n\n");
    printf("Your full name is: ");

    for (rmc_i = 1; rmc_i < argc; rmc_i++) // loop starts from 1, as we don't need the name of executable
    {
        printf("%s ", argv[rmc_i]);
    }
    printf("\n\n");

    return(0);
}

