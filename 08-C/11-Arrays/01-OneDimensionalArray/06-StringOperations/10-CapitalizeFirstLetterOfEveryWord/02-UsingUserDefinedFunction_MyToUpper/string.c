#include <stdio.h>

#define MAX_NAME_LENGTH 512

#define SPACE ' '

#define FULLSTOP '.'
#define COMMA ','
#define EXCLAMATION '!'
#define QUESTION_MARK '?'


int main(void)
{
    // function prototype
    int MyStrlen(char[]);
    char MyToUpper(char);

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
            chArray_capitalizeFirstLetterOfEveryWord_rmc[j_rmc] = MyToUpper(chArray_rmc[i_rmc]);
        else if (chArray_rmc[i_rmc] == SPACE) // first letter of every word must be capital, words are separated by space
        {
            chArray_capitalizeFirstLetterOfEveryWord_rmc[j_rmc] = chArray_rmc[i_rmc];
            chArray_capitalizeFirstLetterOfEveryWord_rmc[j_rmc + 1] = MyToUpper(chArray_rmc[i_rmc + 1]);
            
            // since, already two characters (at index i and i+1) have been considered, we are extra incrementing i and j by 1
            i_rmc++;
            j_rmc++;
        }
        else if ((chArray_rmc[i_rmc] == FULLSTOP || chArray_rmc[i_rmc] == COMMA 
            || chArray_rmc[i_rmc] == EXCLAMATION || chArray_rmc[i_rmc] == QUESTION_MARK) 
            && (chArray_rmc[i_rmc] != SPACE)) // first letter of every word after punctuation mark, in the sentence must be a capital letter. Words are separated by punctuations
        {
            chArray_capitalizeFirstLetterOfEveryWord_rmc[j_rmc] = chArray_rmc[i_rmc];
            chArray_capitalizeFirstLetterOfEveryWord_rmc[j_rmc + 1] = SPACE;
            chArray_capitalizeFirstLetterOfEveryWord_rmc[j_rmc + 2] = MyToUpper(chArray_rmc[i_rmc + 1]);

            // since, already two characters (at index i and i+1) have been considered, we are extra incrementing i by 1
            // since, already three characters (at index j, j+1 and j+2) have been considered, we are extra incrementing j by 2
            j_rmc += 2;
            i_rmc++;
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

char MyToUpper(char ch)
{
    // variable declaration
    int num_rmc;
    int c_rmc;

    // code
    // ASCII VALUE OF 'a' (97) - ASCII VALUE OF 'A' (65) = 32
    // this subtraction will give the exact difference between the upper and lower case counterparts of each letters of alphabet
    // if this difference is subtracted from ASCII value of lower case letter, we will get the ASCII value of its capital letter
    num_rmc = 'a' - 'A';

    if ((int)ch >= 97 && (int)ch <= 122)
    {
        c_rmc = (int)ch - num_rmc;
        return((char)c_rmc);
    }
    else
    {
        return(ch);
    }
}

