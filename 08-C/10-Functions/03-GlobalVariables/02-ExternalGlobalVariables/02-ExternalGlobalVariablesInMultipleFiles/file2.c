// GLOBAL SCOPE
// rmc_global_count is a global variable declared in source code file globalVariables.c
// to access it in this file, it must be first re-declared as an external variable in the global scope of this file along with the 'extern' keyword and its proper data type
// then, it can be used as any ordinary global variable throughtout this file as well
// here, re-declaring rmc_global_count as local variable using 'extern' keyword

#include <stdio.h> 


void change_count_two(void)
{
    // code
    extern int rmc_global_count;
    rmc_global_count += 1;
    printf("change_count_two(): value of global_count in file2.c = %d\n", rmc_global_count);
}

