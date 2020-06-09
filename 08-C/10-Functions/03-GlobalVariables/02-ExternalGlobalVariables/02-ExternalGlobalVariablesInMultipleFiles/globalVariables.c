#include <stdio.h> 

// GLOBAL SCOPE
int rmc_global_count = 0;

int main(void)
{
    // function prototypes
    void change_count(void);
    void change_count_one(void); // function defines in file1.c
    void change_count_two(void); // function defines in file2.c

    // code
    printf("\n");

    change_count();
    change_count_one(); // function defines in file1.c
    change_count_two(); // function defines in file2.c

    return(0);
}

void change_count(void)
{
    // code
    rmc_global_count += 1;
    printf("Global Count = %d\n", rmc_global_count);
}

