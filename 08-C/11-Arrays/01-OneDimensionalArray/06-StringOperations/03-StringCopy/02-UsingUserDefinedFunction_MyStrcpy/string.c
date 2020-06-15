#include <stdio.h>

#define MAX_NAME_LENGTH 512

int main(void)
{
    // function prototype
    void MyStrcpy(char[], char[]);

    // variable declarations
    char chArray_orig_rmc[MAX_NAME_LENGTH], chArray_copy_rmc[MAX_NAME_LENGTH]; // A character array is a string

    // code

    // STRING INPUT
    printf("\n\n");
    printf("Enter a string: \n\n");
    gets_s(chArray_orig_rmc, MAX_NAME_LENGTH);

    // STRING COPY
    MyStrcpy(chArray_copy_rmc, chArray_orig_rmc);

    // STRING OUTPUT
    printf("\n\n");
    printf("The original string entered by you (chArray_orig_rmc[]) is: \n\n");
    printf("%s\n", chArray_orig_rmc);

    printf("\n\n");
    printf("The copied string (chArray_copy_rmc[]) is: \n\n");
    printf("%s\n", chArray_copy_rmc);

    return(0);
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


