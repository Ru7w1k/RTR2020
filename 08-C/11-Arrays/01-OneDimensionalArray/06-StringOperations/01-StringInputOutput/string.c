#include <stdio.h>

#define MAX_NAME_LENGTH 512

int main(void)
{
    // variable declarations
    char chArray_rmc[MAX_NAME_LENGTH]; // A character array is a string

    // code
    printf("\n\n");
    printf("Enter a string: \n\n");
    gets_s(chArray_rmc, MAX_NAME_LENGTH);

    printf("\n\n");
    printf("String Entered by you is: \n\n");
    printf("%s\n", chArray_rmc);
    return(0);
}

