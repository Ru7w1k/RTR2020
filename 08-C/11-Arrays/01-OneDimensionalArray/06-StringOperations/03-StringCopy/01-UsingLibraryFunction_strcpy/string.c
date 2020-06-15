#include <stdio.h>

#define MAX_NAME_LENGTH 512

int main(void)
{
    // variable declarations
    char chArray_orig_rmc[MAX_NAME_LENGTH], chArray_copy_rmc[MAX_NAME_LENGTH]; // A character array is a string

    // code

    // STRING INPUT
    printf("\n\n");
    printf("Enter a string: \n\n");
    gets_s(chArray_orig_rmc, MAX_NAME_LENGTH);

    // STRING COPY
    strcpy(chArray_copy_rmc, chArray_orig_rmc);

    // STRING OUTPUT
    printf("\n\n");
    printf("The original string entered by you (chArray_orig_rmc[]) is: \n\n");
    printf("%s\n", chArray_orig_rmc);

    printf("\n\n");
    printf("The copied string (chArray_copy_rmc[]) is: \n\n");
    printf("%s\n", chArray_copy_rmc);

    return(0);
}

