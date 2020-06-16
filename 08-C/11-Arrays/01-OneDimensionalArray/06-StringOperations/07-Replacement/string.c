// THIS PROGRAM REPLACES ALL VOWELS IN THE INPUT STRING WITH * (asterisk) SYMBOLS
// FOR EXAMPLE: ORIGINAL STRING: 'Rutwik Choughule' WILL BECOME 'R*tw*k Ch**gh*l*'

#include <stdio.h>

#define MAX_NAME_LENGTH 512

int main(void)
{
    // function prototype
    int MyStrlen(char[]);
    void MyStrcpy(char[], char[]);

    // variable declarations
    char chArray_orig_rmc[MAX_NAME_LENGTH], chArray_vowelsReplaced_rmc[MAX_NAME_LENGTH]; // A character array is a string
    int iStringLength_rmc;
    int i_rmc;

    // code

    // STRING INPUT
    printf("\n\n");
    printf("Enter a string: \n\n");
    gets_s(chArray_orig_rmc, MAX_NAME_LENGTH);

    // STRING OUTPUT
    MyStrcpy(chArray_vowelsReplaced_rmc, chArray_orig_rmc);

    iStringLength_rmc = MyStrlen(chArray_vowelsReplaced_rmc);

    for (i_rmc = 0; i_rmc < iStringLength_rmc; i_rmc++)
    {
        switch (chArray_vowelsReplaced_rmc[i_rmc])
        {
        case 'A':
        case 'a':
        case 'E':
        case 'e':
        case 'I':
        case 'i':
        case 'O':
        case 'o':
        case 'U':
        case 'u':
            chArray_vowelsReplaced_rmc[i_rmc] = '*';
            break;

        default:
            break;
        }
    }

    // STRING OUTPUT
    printf("\n\n");
    printf("String Entered by you is: \n\n");
    printf("%s\n", chArray_orig_rmc);

    printf("\n\n");
    printf("String After Replacement of Vowels by * is: \n\n");
    printf("%s\n", chArray_vowelsReplaced_rmc);

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

void MyStrcpy(char str_destination[], char str_source[])
{
    // function prototype
    int MyStrlen(char[]);

    // variable declarations
    int iStringLength_rmc = 0;
    int rmc_j;

    // code
    iStringLength_rmc = MyStrlen(str_source);
    for (rmc_j = 0; rmc_j < iStringLength_rmc; rmc_j++)
        str_destination[rmc_j] = str_source[rmc_j];

    str_destination[rmc_j] = '\0';
}
