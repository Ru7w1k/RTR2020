#include <stdio.h> // contains declaration of printf()

// entry-point function => main() => valid return type (int) and 1 parameters (int argc), hence no command line arguments
int main(int argc)
{
    // code
    printf("\n\n");
    printf("Hello World!\n"); // library function
    printf("Number of command line arguments = %d\n\n", argc);
    return(0);
}

