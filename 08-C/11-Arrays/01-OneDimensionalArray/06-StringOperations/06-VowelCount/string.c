#include <stdio.h>

#define MAX_NAME_LENGTH 512

int main(void)
{
    // function prototype
    void MyStrcat(char[], char[]);

    // variable declarations
    char chArray_rmc[MAX_NAME_LENGTH]; // A character array is a string
    int iStringLength_rmc;
    int countA_rmc = 0, countE_rmc = 0, countI_rmc = 0, countO_rmc = 0, countU_rmc = 0; // initial count = 0
    int i_rmc;

    // code

    // STRING INPUT
    printf("\n\n");
    printf("Enter a string: \n\n");
    gets_s(chArray_rmc, MAX_NAME_LENGTH);

    // STRING OUTPUT
    printf("\n\n");
    printf("String entered by you is: \n\n");
    printf("%s\n", chArray_rmc);

    iStringLength_rmc = MyStrlen(chArray_rmc);

    for (i_rmc = 0; i_rmc < iStringLength_rmc; i_rmc++)
    {
        switch (chArray_rmc[i_rmc])
        {
        case 'A':
        case 'a':
            countA_rmc++;
            break;

        case 'E':
        case 'e':
            countE_rmc++;
            break;

        case 'I':
        case 'i':
            countI_rmc++;
            break;

        case 'O':
        case 'o':
            countO_rmc++;
            break;

        case 'U':
        case 'u':
            countU_rmc++;
            break;

        default:
            break;
        }
    }

    printf("\n\n");
    printf("In the string entered by you, the vowels and the numbers of their occurences are as follows: \n\n");
    printf("'A' has occured = %d times!\n\n", countA_rmc);
    printf("'E' has occured = %d times!\n\n", countE_rmc);
    printf("'I' has occured = %d times!\n\n", countI_rmc);
    printf("'O' has occured = %d times!\n\n", countO_rmc);
    printf("'U' has occured = %d times!\n\n", countU_rmc);

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


