#include <stdio.h> // contains declaration of printf()

// USER DEFINED FUNCTIONS: METHOD OF CALLING FUNCTION 2
// CALLING ONLY TWO FUNCTIONS IN main() DIRECTLY, REST OF THE FUNCTIONS TRACE THEIR CALL INDIRECTLY TO main()

// entry-point function => main() => valid return type (int) and 2 parameters (int argc, char *argv[]), hence no command line arguments
int main(int argc, char *argv[], char *envp[])
{
    // function prototype / declaration / signature
    void display_information(void);
    void Function_Country(void);

    // code
    display_information();
    Function_Country();

    return(0);
}

// User-Defined functions' definitions
void display_information(void) // function definition
{
    // function prototypes
    void Function_My(void);
    void Function_Name(void);
    void Function_Is(void);
    void Function_FirstName(void);
    void Function_MiddleName(void);
    void Function_LastName(void);
    void Function_OfAMC(void);

    // code
    // function calls
    Function_My();
    Function_Name();
    Function_Is();
    Function_FirstName();
    Function_MiddleName();
    Function_LastName();
    Function_OfAMC();
}

void Function_My(void)
{
    // code
    printf("\n\n");
    printf("My");
}

void Function_Name(void)
{
    // code
    printf("\n\n");
    printf("Name");
}

void Function_Is(void)
{
    // code
    printf("\n\n");
    printf("Is");
}

void Function_FirstName(void)
{
    // code
    printf("\n\n");
    printf("Rutwik");
}

void Function_MiddleName(void)
{
    // code
    printf("\n\n");
    printf("Milind");
}

void Function_LastName(void)
{
    // code
    printf("\n\n");
    printf("Choughule");
}

void Function_OfAMC(void)
{
    // code
    printf("\n\n");
    printf("of ASTROMEDICOMP");
}

void Function_Country(void)
{
    // code
    printf("\n\n");
    printf("I live in India.");
    printf("\n\n");
}

