#include <stdio.h> // contains declaration of printf()

// GLOBAL SCOPE

// if not initialized by us, global variables are initializes to their zero values
// (with respect to their data types, i.e. 0 for int, 0.0 for float)
// but still, for good programming discipline, we shall explicitly initialize our global variables with 0

int rmc_global_count = 0;

int main(void)
{
    // function prototypes
    void change_count_one(void);
    void change_count_two(void);
    void change_count_three(void);

    // code
    printf("\n");

    printf("main(): Value of global count = %d\n", rmc_global_count);
    
    change_count_one();
    change_count_two();
    change_count_three();

    printf("\n");
    return(0);
}

// GLOBAL SCOPE
void change_count_one(void)
{
    // code
    rmc_global_count = 100;
    printf("change_count_one(): value of global count = %d\n", rmc_global_count);
}

void change_count_two(void)
{
    // code
    rmc_global_count += 1;
    printf("change_count_two(): value of global count = %d\n", rmc_global_count);
}

void change_count_three(void)
{
    // code
    rmc_global_count += 10;
    printf("change_count_three(): value of global count = %d\n", rmc_global_count);
}

