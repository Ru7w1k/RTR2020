#include <stdio.h> // contains declaration of printf()

// USER DEFINED FUNCTIONS: METHOD OF CALLING FUNCTION 3
// CALLING ONLY ONE FUNCTIONS IN main() DIRECTLY, REST OF THE FUNCTIONS TRACE THEIR CALL INDIRECTLY TO main()

// entry-point function => main() => valid return type (int) and 2 parameters (int argc, char *argv[]), hence no command line arguments
int main(int argc, char *argv[], char *envp[])
{
    // function prototype / declaration / signature
    void Function_Country(void);

    // code
    Function_Country();

    return(0);
}

// User-Defined functions' definitions
void Function_Country(void)
{
    // function declarations
    void Function_OfAMC(void);

    // code
    Function_OfAMC();

    printf("\n\n");
    printf("I live in India.");
    printf("\n\n");
}

void Function_OfAMC(void)
{
    // function declarations
    void Function_Surname(void);

    // code
    Function_Surname();

    printf("\n\n");
    printf("of ASTROMEDICOMP");
}

void Function_Surname(void)
{
    // function declarations
    void Function_MiddleName(void);

    // code
    Function_MiddleName();

    printf("\n\n");
    printf("Choughule");
}

void Function_MiddleName(void)
{
    // function declarations
    void Function_FirstName(void);

    // code
    Function_FirstName();

    printf("\n\n");
    printf("Milind");
}

void Function_FirstName(void)
{
    // function declarations
    void Function_Is(void);

    // code
    Function_Is();

    printf("\n\n");
    printf("Rutwik");
}

void Function_Is(void)
{
    // function name
    void Function_Name(void);
    
    // code
    Function_Name();

    printf("\n\n");
    printf("Is");
}

void Function_Name(void)
{
    // function declarations
    void Function_My(void);

    // code
    Function_My();

    printf("\n\n");
    printf("Name");
}

void Function_My(void)
{
    // code
    printf("\n\n");
    printf("My");
}


