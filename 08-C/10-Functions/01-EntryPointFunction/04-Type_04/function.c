#include <stdio.h> // contains declaration of printf()

// entry-point function => main() => valid return type (int) and 2 parameters (int argc, char *argv[]), hence no command line arguments
int main(int argc, char *argv[])
{
    // variable declarations
    int rmc_i;

    // code
    printf("\n\n");
    printf("Hello World!\n"); // library function
    printf("Number of command line arguments = %d\n\n", argc);

    printf("Command line arguments passed to this program are: \n\n");
    for (rmc_i = 0; rmc_i < argc; rmc_i++)
    {
        printf("Command line argument number %d = %s\n", (rmc_i+1), argv[rmc_i]);
    }
    printf("\n\n");

    return(0);
}

