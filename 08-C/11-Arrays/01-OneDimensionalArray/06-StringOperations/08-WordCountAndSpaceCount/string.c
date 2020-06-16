#include <stdio.h>

#define MAX_NAME_LENGTH 512

int main(void)
{
    // function prototype
    int MyStrlen(char[]);

    // variable declarations
    char chArray_rmc[MAX_NAME_LENGTH]; // A character array is a string
    int iStringLength_rmc;
    int i_rmc;
    int word_count_rmc = 0, space_count_rmc = 0;

    // code

    // STRING INPUT
    printf("\n\n");
    printf("Enter a string: \n\n");
    gets_s(chArray_rmc, MAX_NAME_LENGTH);

    iStringLength_rmc = MyStrlen(chArray_rmc);

    for (i_rmc = 0; i_rmc < iStringLength_rmc; i_rmc++)
    {
        switch (chArray_rmc[i_rmc])
        {
        case 32: // 32 is ASCII VALUE OF SPACE ' ' CHARACTER
            space_count_rmc++;
            break;
        
        default:
            break;
        }
    }

    word_count_rmc = space_count_rmc + 1;

    // STRING OUTPUT
    printf("\n\n");
    printf("String Entered by you is: \n\n");
    printf("%s\n", chArray_rmc);

    printf("\n\n");
    printf("Number of spaces in the input string: %d \n\n", space_count_rmc);
    printf("Number of words in the input string: %d \n\n", word_count_rmc);

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
