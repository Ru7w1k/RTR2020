#include <stdio.h>
#include <string.h> // for strrev()

#define MAX_NAME_LENGTH 512

int main(void)
{
    // variable declarations
    char chArray_orig_rmc[MAX_NAME_LENGTH]; // A character array is a string

    // code

    // STRING INPUT
    printf("\n\n");
    printf("Enter a string: \n\n");
    gets_s(chArray_orig_rmc, MAX_NAME_LENGTH);

    // STRING OUTPUT
    printf("\n\n");
    printf("The original string entered by you (chArray_orig_rmc[]) is: \n\n");
    printf("%s\n", chArray_orig_rmc);

    printf("\n\n");
    printf("The reversed string (chArray_reverse_rmc[]) is: \n\n");
    printf("%s\n", strrev(chArray_orig_rmc));

    return(0);
}

