#include <stdio.h>

#define MAX_NAME_LENGTH 512

int main(void)
{
    // function prototype
    int MyStrlen(char[]);

    // variable declarations
    char chArray_rmc[MAX_NAME_LENGTH], chArray_spaceRemoved_rmc[MAX_NAME_LENGTH]; // A character array is a string
    int iStringLength_rmc;
    int i_rmc, j_rmc;

    // code

    // STRING INPUT
    printf("\n\n");
    printf("Enter a string: \n\n");
    gets_s(chArray_rmc, MAX_NAME_LENGTH);

    iStringLength_rmc = MyStrlen(chArray_rmc);
    j_rmc = 0;
    for (i_rmc = 0; i_rmc < iStringLength_rmc; i_rmc++)
    {
        if (chArray_rmc[i_rmc] == ' ')
            continue;
        else
        {
            chArray_spaceRemoved_rmc[j_rmc] = chArray_rmc[i_rmc];
            j_rmc++;
        }
    }
    chArray_spaceRemoved_rmc[j_rmc] = '\0';

    // STRING OUTPUT
    printf("\n\n");
    printf("String Entered by you is: \n\n");
    printf("%s\n", chArray_rmc);

    printf("\n\n");
    printf("String After removal of space is: \n\n");
    printf("%s\n", chArray_spaceRemoved_rmc);

    return(0);
}

int MyStrlen(char str[])
{
    // variable declarations
    int rmc_j;
    int rmc_string_length = 0;

    // code
    // detecting exact length of the string, by detecting the first occurence of null-terminating character (\0)
    for (rmc_j = 0; rmc_j < MAX_NAME_LENGTH; rmc_j++)
    {
        if (str[rmc_j] == '\0')
            break;
        else
            rmc_string_length++;
    }
    
    return(rmc_string_length);
}
