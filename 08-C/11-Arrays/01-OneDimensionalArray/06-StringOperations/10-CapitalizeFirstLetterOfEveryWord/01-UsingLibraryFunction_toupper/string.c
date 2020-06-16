#include <stdio.h>
#include <ctype.h> // for toupper()

#define MAX_NAME_LENGTH 512

int main(void)
{
    // function prototype
    int MyStrlen(char[]);

    // variable declarations
    char chArray_rmc[MAX_NAME_LENGTH], chArray_capitalizeFirstLetterOfEveryWord_rmc[MAX_NAME_LENGTH]; // A character array is a string
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
        if (i_rmc == 0) // first letter of any sentence must be capital letter
            chArray_capitalizeFirstLetterOfEveryWord_rmc[j_rmc] = toupper(chArray_rmc[i_rmc]);
        else if (chArray_rmc[i_rmc] == ' ') // first letter of every word must be capital, words are separated by space
        {
            chArray_capitalizeFirstLetterOfEveryWord_rmc[j_rmc] = chArray_rmc[i_rmc];
            chArray_capitalizeFirstLetterOfEveryWord_rmc[j_rmc + 1] = toupper(chArray_rmc[i_rmc + 1]);
            
            // since, already two characters (at index i and i+1) have been considered, we are extra incrementing i and j by 1
            i_rmc++;
            j_rmc++;
        }
        else
            chArray_capitalizeFirstLetterOfEveryWord_rmc[j_rmc] = chArray_rmc[i_rmc];

        j_rmc++;
    }
    chArray_capitalizeFirstLetterOfEveryWord_rmc[j_rmc] = '\0';

    // STRING OUTPUT
    printf("\n\n");
    printf("String Entered by you is: \n\n");
    printf("%s\n", chArray_rmc);

    printf("\n\n");
    printf("String After Capitalizing First Letter of Every Word is: \n\n");
    printf("%s\n", chArray_capitalizeFirstLetterOfEveryWord_rmc);

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

