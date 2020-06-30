#include <stdio.h>
#include <stdlib.h>

#define MAX_NAME_LENGTH 512

int main(void)
{
    // function prototype
    int MyStrlen(char *);

    // variable declarations
    char *chArray_rmc = NULL; // character array can be represented by a char pointer to mark the base address (char *)
    int iStringLength_rmc = 0;

    // code
    printf("\n\n");
    chArray_rmc = (char *)malloc(MAX_NAME_LENGTH * sizeof(char));
    if (chArray_rmc == NULL)
    {
        printf("MEMORY ALLOCATION TO CHARACTER ARRAY FAILED! Exiting now...\n\n");
        exit(0);
    }

    // STRING INPUT
    printf("Enter a string: \n\n");
    gets_s(chArray_rmc, MAX_NAME_LENGTH);

    // STRING OUTPUT
    printf("\n\n");
    printf("String Entered by you is: \n\n");
    printf("%s\n", chArray_rmc);

    // STRING LENGTH
    printf("\n\n");
    iStringLength_rmc = MyStrlen(chArray_rmc);
    printf("Length of string is = %d characters!\n\n", iStringLength_rmc);

    if (chArray_rmc)
    {
        free(chArray_rmc);
        chArray_rmc = NULL;
    }

    return(0);
}

int MyStrlen(char *str)
{
    // variable declarations
    int rmc_j;
    int rmc_string_length = 0;

    // code
    // detecting exact length of the string, by detecting the first occurence of null-terminating character (\0)
    for (rmc_j = 0; rmc_j < MAX_NAME_LENGTH; rmc_j++)
    {
        if (*(str + rmc_j) == '\0')
            break;
        else
            rmc_string_length++;
    }
    
    return(rmc_string_length);
}

