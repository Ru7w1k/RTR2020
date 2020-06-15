#include <stdio.h>

#define MAX_NAME_LENGTH 512

int main(void)
{
    // variable declarations
    char chArray_one_rmc[MAX_NAME_LENGTH], chArray_two_rmc[MAX_NAME_LENGTH]; // A character array is a string

    // code

    // STRING INPUT
    printf("\n\n");
    printf("Enter a string: \n\n");
    gets_s(chArray_one_rmc, MAX_NAME_LENGTH);

    printf("\n\n");
    printf("Enter second string: \n\n");
    gets_s(chArray_two_rmc, MAX_NAME_LENGTH);

    // STRING CONCAT
    printf("\n\n");
    printf("****** BEFORE CONCATENATION ******");
    printf("\n\n");
    printf("The original first string entered by you (chArray_one_rmc[]) is: \n\n");
    printf("%s\n", chArray_one_rmc);

    printf("\n\n");
    printf("The original second string entered by you (chArray_two_rmc[]) is: \n\n");
    printf("%s\n", chArray_two_rmc);

    strcat(chArray_one_rmc, chArray_two_rmc);

    printf("\n\n");
    printf("****** AFTER CONCATENATION ******");
    printf("\n\n");
    printf("chArray_one_rmc[] is: \n\n");
    printf("%s\n", chArray_one_rmc);

    printf("\n\n");
    printf("chArray_two_rmc[] is: \n\n");
    printf("%s\n", chArray_two_rmc);

    return(0);
}

