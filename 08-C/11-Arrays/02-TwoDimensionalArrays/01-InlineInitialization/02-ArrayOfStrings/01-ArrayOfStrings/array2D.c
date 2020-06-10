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
    int rmc_char_size;
    int rmc_strArray_size;
    int rmc_strArray_num_elements, rmc_strArray_num_rows, rmc_strArray_num_columns;
    int rmc_strActual_num_chars = 0;
    int rmc_i;

    // code
    printf("\n\n");

    rmc_char_size = sizeof(char);

    rmc_strArray_size = sizeof(rmc_strArray);
    printf("Size of Two Dimensional (2D) Character array (String Array) is = %d\n\n", rmc_strArray_size);

    rmc_strArray_num_rows = rmc_strArray_size / sizeof(rmc_strArray[0]);
    printf("Number of Rows in Two Dimensional (2D) Character array (String Array) is = %d\n\n", rmc_strArray_num_rows);

    rmc_strArray_num_columns = sizeof(rmc_strArray[0]) / rmc_char_size;
    printf("Number of Columns in Two Dimensional (2D) Character array (String Array) is = %d\n\n", rmc_strArray_num_columns);

    rmc_strArray_num_elements = rmc_strArray_num_rows * rmc_strArray_num_columns;
    printf("Number of Elements in Two Dimensional (2D) Character array (String Array) is = %d\n\n", rmc_strArray_num_elements);

    for (rmc_i = 0; rmc_i < rmc_strArray_num_rows; rmc_i++)
    {
        rmc_strActual_num_chars = rmc_strActual_num_chars + MyStrlen(rmc_strArray[rmc_i]);
    }

    printf("Actual number of elements (characters) in two dimensional (2D) character array (string array) is = %d\n\n", rmc_strActual_num_chars);

    printf("\n\n");
    printf("Strings in the 2D array: \n\n");

    // since char[][] is an array of strings, referencing only by the row number (first []) will give the row or the string
    // the column number (second []) is the particular in that string/row
    printf("%s ", rmc_strArray[0]);
    printf("%s ", rmc_strArray[1]);
    printf("%s ", rmc_strArray[2]);
    printf("%s ", rmc_strArray[3]);
    printf("%s ", rmc_strArray[4]);
    printf("%s ", rmc_strArray[5]);
    printf("%s ", rmc_strArray[6]);
    printf("%s ", rmc_strArray[7]);
    printf("%s ", rmc_strArray[8]);
    printf("%s\n\n", rmc_strArray[9]);

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

