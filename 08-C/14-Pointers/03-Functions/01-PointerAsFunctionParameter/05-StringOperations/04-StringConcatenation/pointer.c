#include <stdio.h>
#include <stdlib.h>

#define MAX_NAME_LENGTH 512

int main(void)
{
    // function prototype
    void MyStrcat(char *, char *);
    int  MyStrlen(char *);

    // variable declarations
    char *chArray_one_rmc = NULL, *chArray_two_rmc = NULL;

    // code
    printf("\n\n");
    chArray_one_rmc = (char *)malloc(MAX_NAME_LENGTH * sizeof(char));
    if (chArray_one_rmc == NULL)
    {
        printf("MEMORY ALLOCATION FOR FIRST STRING FAILED! Exiting now...\n\n");
        exit(0);
    }

    printf("Enter a string: \n\n");
    gets_s(chArray_one_rmc, MAX_NAME_LENGTH);

    printf("\n\n");
    chArray_two_rmc = (char *)malloc(MAX_NAME_LENGTH * sizeof(char));
    if (chArray_two_rmc == NULL)
    {
        printf("MEMORY ALLOCATION FOR SECOND STRING FAILED! Exiting now...\n\n");
        exit(0);
    }
    printf("Enter second string: \n\n");
    gets_s(chArray_two_rmc, MAX_NAME_LENGTH);

    // STRING CONCAT
    printf("\n\n");
    printf("****** BEFORE CONCATENATION ******");
    printf("\n\n");
    printf("The original first string entered by you (chArray_one_rmc[]) is: \n\n");
    printf("%s\n", chArray_one_rmc);

    printf("\n\n");
    printf("The original second string entered by you (chArray_two_rmc[]) is: \n\n");
    printf("%s\n", chArray_two_rmc);

    MyStrcat(chArray_one_rmc, chArray_two_rmc);

    printf("\n\n");
    printf("****** AFTER CONCATENATION ******");
    printf("\n\n");
    printf("chArray_one_rmc[] is: \n\n");
    printf("%s\n", chArray_one_rmc);

    printf("\n\n");
    printf("chArray_two_rmc[] is: \n\n");
    printf("%s\n", chArray_two_rmc);

    if (chArray_two_rmc)
    {
        free(chArray_two_rmc);
        chArray_two_rmc = NULL;
        printf("\n\n");
        printf("MEMORY ALLOCATED FOR chArray_two_rmc HAS BEEN FREED!!\n\n");
    }

    if (chArray_one_rmc)
    {
        free(chArray_one_rmc);
        chArray_one_rmc = NULL;
        printf("\n\n");
        printf("MEMORY ALLOCATED FOR chArray_one_rmc HAS BEEN FREED!!\n\n");
    }

    return(0);
}

void MyStrcat(char str_destination[], char str_source[])
{
    // function prototype
    int MyStrlen(char *);

    // variable declarations
    int iStringLength_src_rmc = 0, iStringLength_dst_rmc = 0;
    int rmc_i, rmc_j, rmc_len;

    // code
    iStringLength_src_rmc = MyStrlen(str_source);
    iStringLength_dst_rmc = MyStrlen(str_destination);

    // array indices begin from 0, hence, last valid index of array will always be (length - 1)
    // so, concatination must begin from index number equal to length of the array 'str_destination'
    // we need to put the character which is at first index of str_source to the (last index + 1) of 'str_destination'
    for (rmc_i = iStringLength_dst_rmc, rmc_j = 0; rmc_j < iStringLength_src_rmc; rmc_i++, rmc_j++)
        *(str_destination + rmc_i) = *(str_source + rmc_j);

    *(str_destination + rmc_i) = '\0';
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


