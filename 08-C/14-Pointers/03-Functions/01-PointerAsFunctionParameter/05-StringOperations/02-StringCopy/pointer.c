#include <stdio.h>
#include <stdlib.h>

#define MAX_NAME_LENGTH 512

int main(void)
{
    // function prototype
    void MyStrcpy(char *, char *);
    int  MyStrlen(char *);

    // variable declarations
    char *chArray_orig_rmc = NULL, *chArray_copy_rmc = NULL;
    int orig_str_len_rmc;

    // code

    // STRING INPUT
    printf("\n\n");
    chArray_orig_rmc = (char *)malloc(MAX_NAME_LENGTH * sizeof(char));
    if (chArray_orig_rmc == NULL)
    {
        printf("MEMORY ALLOCATION FOR ORIGINAL STRING FAILED! Exiting now...\n\n");
        exit(0);
    }

    printf("Enter a string: \n\n");
    gets_s(chArray_orig_rmc, MAX_NAME_LENGTH);

    orig_str_len_rmc = MyStrlen(chArray_orig_rmc);
    chArray_copy_rmc = (char *)malloc(orig_str_len_rmc * sizeof(char));
    if (chArray_copy_rmc == NULL)
    {
        printf("MEMORY ALLOCATION FAILED FOR COPIED STRING! Exiting now..\n\n");
        exit(0);
    }

    // STRING COPY
    MyStrcpy(chArray_copy_rmc, chArray_orig_rmc);

    // STRING OUTPUT
    printf("\n\n");
    printf("The original string entered by you (chArray_orig_rmc[]) is: \n\n");
    printf("%s\n", chArray_orig_rmc);

    printf("\n\n");
    printf("The copied string (chArray_copy_rmc[]) is: \n\n");
    printf("%s\n\n", chArray_copy_rmc);

    if (chArray_orig_rmc)
    {
        free(chArray_orig_rmc);
        chArray_orig_rmc = NULL;
        printf("MEMORY ALLOCATED FOR chArray_orig_rmc HAS BEEN FREED!!\n\n");
    }
    if (chArray_copy_rmc)
    {
        free(chArray_copy_rmc);
        chArray_copy_rmc = NULL;
        printf("MEMORY ALLOCATED FOR chArray_copy_rmc HAS BEEN FREED!!\n\n");
    }

    return(0);
}

void MyStrcpy(char *str_destination, char *str_source)
{
    // function prototype
    int MyStrlen(char *);

    // variable declarations
    int iStringLength_rmc = 0;
    int rmc_j;

    // code
    iStringLength_rmc = MyStrlen(str_source);
    for (rmc_j = 0; rmc_j < iStringLength_rmc; rmc_j++)
        *(str_destination + rmc_j) = *(str_source + rmc_j);

    *(str_destination + rmc_j) = '\0';
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

