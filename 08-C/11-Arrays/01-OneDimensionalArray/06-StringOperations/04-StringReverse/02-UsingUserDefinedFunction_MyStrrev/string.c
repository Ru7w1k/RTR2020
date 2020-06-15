#include <stdio.h>

#define MAX_NAME_LENGTH 512

int main(void)
{
    // function prototype
    void MyStrrev(char[], char[]);

    // variable declarations
    char chArray_orig_rmc[MAX_NAME_LENGTH], chArray_reversed_rmc[MAX_NAME_LENGTH]; // A character array is a string

    // code

    // STRING INPUT
    printf("\n\n");
    printf("Enter a string: \n\n");
    gets_s(chArray_orig_rmc, MAX_NAME_LENGTH);

    // STRING COPY
    MyStrrev(chArray_reversed_rmc, chArray_orig_rmc);

    // STRING OUTPUT
    printf("\n\n");
    printf("The original string entered by you (chArray_orig_rmc[]) is: \n\n");
    printf("%s\n", chArray_orig_rmc);

    printf("\n\n");
    printf("The reversed string (chArray_reversed_rmc[]) is: \n\n");
    printf("%s\n", chArray_reversed_rmc);

    return(0);
}

void MyStrrev(char str_destination[], char str_source[])
{
    // function prototype
    int MyStrlen(char[]);

    // variable declarations
    int iStringLength_rmc = 0;
    int rmc_i, rmc_j, rmc_len;

    // code
    iStringLength_rmc = MyStrlen(str_source);

    // array indices begin from 0, hence, last index will always be (length - 1)
    rmc_len = iStringLength_rmc - 1;

    // we need to put the character which is at last index of 'str_source' to the first index of 'str_destination'
    // and second-last character of str_source to the second character of str_destination and so on..
    for (rmc_i = 0, rmc_j = rmc_len; rmc_i < iStringLength_rmc, rmc_j >= 0; rmc_i++, rmc_j--)
        str_destination[rmc_i] = str_source[rmc_j];

    str_destination[rmc_i] = '\0';
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


