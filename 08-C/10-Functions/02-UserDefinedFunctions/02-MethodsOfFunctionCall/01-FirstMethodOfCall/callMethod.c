#include <stdio.h> // contains declaration of printf()

// USER DEFINED FUNCTIONS: METHOD OF CALLING FUNCTION 1
// CALLING ALL FUNCTIONS IN main() DIRECTLY

// entry-point function => main() => valid return type (int) and 2 parameters (int argc, char *argv[]), hence no command line arguments
int main(int argc, char *argv[], char *envp[])
{
    // function prototype / declaration / signature
    void MyAddition(void);
    int MySubtraction(void);
    void MyMultiplication(int, int);
    int MyDivision(int, int);

    // variable declaration: local variables to main()
    int rmc_result_subtraction;
    int rmc_a_multiplication, rmc_b_multiplication;
    int rmc_a_division, rmc_b_division, rmc_result_division;
        
    // code

    // ADDITION
    MyAddition(); // function call

    // SUBTRACTION
    rmc_result_subtraction = MySubtraction();
    printf("\n\n");
    printf("Subtraction yeilds result: %d", rmc_result_subtraction); // function call

    // MULTIPLICATION
    printf("\n\n");
    printf("Enter Integer value for 'A' for multiplication: ");
    scanf("%d", &rmc_a_multiplication);

    printf("\n\n");
    printf("Enter Integer value for 'B' for multiplication: ");
    scanf("%d", &rmc_b_multiplication);

    MyMultiplication(rmc_a_multiplication, rmc_b_multiplication); // function call

    // DIVISION
    printf("\n\n");
    printf("Enter Integer value for 'A' for division: ");
    scanf("%d", &rmc_a_division);

    printf("\n\n");
    printf("Enter Integer value for 'B' for division: ");
    scanf("%d", &rmc_b_division);

    rmc_result_division = MyDivision(rmc_a_division, rmc_b_division); // function call
    printf("\n\n");
    printf("Division of %d and %d gives = %d: ", rmc_a_division, rmc_b_division, rmc_result_division);

    printf("\n\n");
    return(0);
}

// function definition of MyAddition()
void MyAddition(void)
{
    // variable declarations: local variables to MyAddition()
    int rmc_a, rmc_b, rmc_sum;

    // code
    printf("\n\n");
    printf("Enter Integer value for 'A' for Addition: ");
    scanf("%d", &rmc_a);

    printf("\n\n");
    printf("Enter Integer value for 'B' for Addition: ");
    scanf("%d", &rmc_b);

    rmc_sum = rmc_a + rmc_b;
    printf("\n\n");
    printf("Sum of %d and %d = %d: ", rmc_a, rmc_b, rmc_sum);
}

// function definition of MySubtraction()
int MySubtraction(void)
{
    // variable declarations: local variables to MySubtraction()
    int rmc_a, rmc_b, rmc_subtraction;

    // code
    printf("\n\n");
    printf("Enter Integer value for 'A' for Subtraction: ");
    scanf("%d", &rmc_a);

    printf("\n\n");
    printf("Enter Integer value for 'B' for Subtraction: ");
    scanf("%d", &rmc_b);

    rmc_subtraction = rmc_a - rmc_b;
    return(rmc_subtraction);
}

// function definition of MyMultiplication()
void MyMultiplication(int a, int b)
{
    // variable declarations: local variables to MyMultiplication()
    int rmc_multiplication;

    // code
    rmc_multiplication = a * b;
    printf("\n\n");
    printf("Multiplication of %d and %d = %d: ", a, b, rmc_multiplication);
}

// function definition of MyDivision()
int MyDivision(int a, int b)
{
    // variable declarations: local variables to MyDivision()
    int rmc_division_quotient;

    // code
    if (a > b)
        rmc_division_quotient = a / b;
    else
        rmc_division_quotient = b / a;
    
    return(rmc_division_quotient);
}



