#include <stdio.h> 

// GLOBAL SCOPE

int main(void)
{
    // LOCAL SCOPE OF main() begins 

    // variable declarations
    // 'rmc_a' is local variable, it is local to main() only
    int rmc_a = 10;

    // function prototypes
    void change_count(void);

    // code
    printf("\n");
    printf("A = %d\n\n", rmc_a);

    // local_count is initialized to 0
    // local_count = local_count + 1 = 0 + 1 = 1
    change_count();

    // since 'local_count' is an ordinary local variable of change_count(), it will NOT retain its value from previous call to change_count()
    // so local_count is AGAIN initialized to 0
    // local_count = local_count + 1 = 0 + 1 = 1
    change_count();

    return(0);
    // LOCAL SCOPE of main() ends
}

// GLOBAL SCOPE
void change_count(void)
{
    // LOCAL SCOPE OF change_count() begins

    // variable declarations
    // 'rmc_local_count' is local variable. It is local to change_count() only
    int rmc_local_count = 0;

    // code
    rmc_local_count = rmc_local_count + 1;
    printf("Local count = %d\n", rmc_local_count);

    // LOCAL SCOPE OF change_count() ends
}

