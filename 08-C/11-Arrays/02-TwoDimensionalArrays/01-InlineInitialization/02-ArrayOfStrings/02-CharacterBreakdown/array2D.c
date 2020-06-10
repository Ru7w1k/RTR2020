#include <stdio.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
    // function prototype
    int MyStrlen(char []);

    // variable declarations

    // A STRING IS AN ARRAY OF CHARACTERS..so char[] IS A char ARRAY AND HENCE, char[] IS A STRING
    // AN ARRAY OF char ARRAYS IS AN ARRAY OF STRINGS!
    // HENCE, char[] IS ONE char ARRAY AND HENCE, IS ONE STRING
    // HENCE, char[][] IS AN ARRAY OF char ARRAYS AND HENCE, IS AN ARRAY OF STRINGS

    // here, the string array can allow a maximum number of 10 strings (10 rows) and each of these 10 strings can have only upto 15 characters maximum (15 columns)
    char rmc_strArray[10][15] = {"Hello!", "Welcome","To", "Real", "Time", "Rendering", "Batch", "(2020-21)", "Of", "ASTRIMEDICOMP"}; // IN-LINE INITIALIZATION
    int rmc_iStrLengths[10]; // 1D Integer array- stores lengths of those strings at corrosponding indices in strArray[], e.g. iStrLength[0] will be the length of string at strArray[0], iStrLength[1] will be the length of string at strArray[1]

    int rmc_strArray_size;
    int rmc_strArray_num_rows;
    int rmc_i, rmc_j;

    // code
    rmc_strArray_size = sizeof(rmc_strArray);
    rmc_strArray_num_rows = rmc_strArray_size / sizeof(rmc_strArray[0]);

    // storing length of all the strings..
    for (rmc_i = 0; rmc_i < rmc_strArray_num_rows; rmc_i++)
        rmc_iStrLengths[rmc_i] = MyStrlen(rmc_strArray[rmc_i]);

    printf("\n\n");
    printf("The entire string array: \n\n");
    for (rmc_i = 0; rmc_i < rmc_strArray_num_rows; rmc_i++)
        printf("%s ", rmc_strArray[rmc_i]);

    printf("\n\n");
    printf("Strings in 2D array: \n\n");
    
    // since, char[][] is an array of strings, referencing only by the row number (first []) will give the row or the string
    // the column number (second []) is the particular character in that string/row

    for (rmc_i = 0; rmc_i < rmc_strArray_num_rows; rmc_i++)
    {
        printf("Strings Number %d = %s\n\n", (rmc_i + 1), rmc_strArray[rmc_i]);
        for (rmc_j = 0; rmc_j < rmc_iStrLengths[rmc_i]; rmc_j++)
        {
            printf("Character %d = %c\n\n", (rmc_j + 1), rmc_strArray[rmc_i][rmc_j]);
        }
        printf("\n\n");
    }

    return(0);
}

int MyStrlen(char str[])
{
    // variable declaration
    int rmc_j;
    int rmc_string_length = 0;

    // code
    // DETERMINING EXACT LENGTH OF THE STRING, BY DETECTING THE FIRST OCCURANCE OF NULL TERMINATING CHARACTER(\0)
    for (rmc_j = 0; rmc_j < MAX_STRING_LENGTH; rmc_j++)
    {
        if (str[rmc_j] == '\0')
            break;
        else
        {
            rmc_string_length++;
        }
    }
    return(rmc_string_length);
}

