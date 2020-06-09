#include <stdio.h> // contains declaration of printf()

// entry-point function => main() => valid return type (int) and 2 parameters (int argc, char *argv[]), hence no command line arguments
int main(int argc, char *argv[], char *envp[])
{
    // function prototype / declaration / signature
    void MyAddition(int, int);

    // variable declaration: local variables to main()
    int rmc_a, rmc_b;
    
    // code
    printf("\n\n");
    printf("Enter Integer value for 'A': ");
    scanf("%d", &rmc_a);

    printf("\n\n");
    printf("Enter Integer value for 'B': ");
    scanf("%d", &rmc_b);

    MyAddition(rmc_a, rmc_b); // function call

    return(0);
}

// USER DEFINED FUNCTION: METHOD OF DEFINITION 3
// NO RETURN VALUE, VALID PARAMETERS (int, int)
void MyAddition(int a, int b)
{
    // variable declarations: local variables to MyAddition()
    int rmc_sum;

    // code
    rmc_sum = a + b;
    printf("\n\n");
    printf("Sum of %d and %d = %d\n\n", a, b, rmc_sum);
}

