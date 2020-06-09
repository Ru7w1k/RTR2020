#include <stdio.h> 

// GLOBAL SCOPE

int main(int argc, char *argv[], char *envp[])
{
    // variable declarations
    unsigned int rmc_num;

    // function prototype
    void recursive(unsigned int);

    // code
    printf("\n\n");
    printf("Entry any number: \n\n");
    scanf("%u", &rmc_num);

    printf("\n\n");
    printf("Output of recursive function: \n\n");

    recursive(rmc_num);

    return(0);
}

void recursive(unsigned int n)
{
    // code
    printf("n = %d\n", n);

    if (n > 0)
    {
        recursive(n-1);
    }
}

