#include <stdio.h>

#define MAX_NAME_LENGTH 512

int main(void)
{
    // variable declarations
    char chArray_rmc[MAX_NAME_LENGTH]; // A character array is a string
    int iStringLength_rmc = 0;

    // code

    // STRING INPUT
    printf("\n\n");
    printf("Enter a string: \n\n");
    gets_s(chArray_rmc, MAX_NAME_LENGTH);

    // STRING OUTPUT
    printf("\n\n");
    printf("String Entered by you is: \n\n");
    printf("%s\n", chArray_rmc);

    // STRING LENGTH
    printf("\n\n");
    iStringLength_rmc = strlen(chArray_rmc);
    printf("Length of string is = %d characters!\n\n", iStringLength_rmc);

    return(0);
}

