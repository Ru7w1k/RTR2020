#include <stdio.h> // contains declaration of printf()

// entry-point function => main() => valid return type (int) and 2 parameters (int argc, char *argv[]), hence no command line arguments
int main(int argc, char *argv[], char *envp[])
{
    // function prototype / declaration / signature
    int MyAddition(void);

    // variable declaration: local variables to main()
    int rmc_result;
    
    // code
    rmc_result = MyAddition(); // function call

    printf("\n\n");
    printf("Sum = %d\n\n", rmc_result);

    return(0);
}

// USER DEFINED FUNCTION: METHOD OF DEFINITION 2
// VALID (int) RETURN VALUE, NO PARAMETERS
int MyAddition(void)
{
    // variable declarations: local variables to MyAddition()
    int rmc_a, rmc_b, rmc_sum;

    // code
    printf("\n\n");
    printf("Enter Integer value for 'A': ");
    scanf("%d", &rmc_a);

    printf("\n\n");
    printf("Enter Integer value for 'B': ");
    scanf("%d", &rmc_b);

    rmc_sum = rmc_a + rmc_b;
    return(rmc_sum);    
}

