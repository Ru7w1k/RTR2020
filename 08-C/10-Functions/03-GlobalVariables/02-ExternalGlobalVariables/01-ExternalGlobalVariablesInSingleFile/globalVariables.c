#include <stdio.h> 

// GLOBAL SCOPE

int main(void)
{
    // function prototypes
    void change_count(void);

    // variable declarations
    extern int rmc_global_count;

    // code
    printf("\n");

    printf("Value of global_count before change_count() = %d\n", rmc_global_count);
    change_count();
    printf("Value of global_count after change_count() = %d\n", rmc_global_count);

    printf("\n");
    return(0);
}

// GLOBAL SCOPE
// rmc_global_count is global variable
// since, it is declared before change_count(), it can be accessed and used as any ordinary global variable in change_count()
// since, it is declared after main(), it must be first re-declared in main() as an external global variable by means of the 'extern' keyword and type of variable
int rmc_global_count = 0;

void change_count(void)
{
    // code
    rmc_global_count = 5;
    printf("value of global_count in change_count() = %d\n", rmc_global_count);
}

