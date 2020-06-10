#include <stdio.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
    // function prototype
    void MyStrcpy(char [], char[]);

    // variable declarations

    // A STRING IS AN ARRAY OF CHARACTERS..so char[] IS A char ARRAY AND HENCE, char[] IS A STRING
    // AN ARRAY OF char ARRAYS IS AN ARRAY OF STRINGS!
    // HENCE, char[] IS ONE char ARRAY AND HENCE, IS ONE STRING
    // HENCE, char[][] IS AN ARRAY OF char ARRAYS AND HENCE, IS AN ARRAY OF STRINGS

    // here, the string array can allow a maximum number of 5 strings (10 rows) and each of these 10 strings can have only upto 10 characters maximum (10 columns)
    char rmc_strArray[5][10]; // 5 ROWS (0, 1, 2, 3, 4) -> 5 STRING (EACH CAN HAVE MAXIMUM 10 CHARACTERS)
    int rmc_char_size;
    int rmc_strArray_size;
    int rmc_strArray_num_elements, rmc_strArray_num_rows, rmc_strArray_num_columns;
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

    // PIECE-MEAL ASSIGNMENT
    MyStrcpy(rmc_strArray[0], "My");
    MyStrcpy(rmc_strArray[1], "Name");
    MyStrcpy(rmc_strArray[2], "Is");
    MyStrcpy(rmc_strArray[3], "Rutwik");
    MyStrcpy(rmc_strArray[4], "Choughule");

    printf("\n\n");
    printf("Strings in the 2D array: \n\n");

    for (rmc_i = 0; rmc_i < rmc_strArray_num_rows; rmc_i++)
        printf("%s ", rmc_strArray[rmc_i]);

    return(0);
}

void MyStrcpy(char str_destination[], char str_source[])
{
    // function declarations
    int MyStrlen(char []);

    // variable declarations
    int rmc_iStringLength = 0;
    int rmc_j;

    // code
    rmc_iStringLength = MyStrlen(str_source);
    for (rmc_j = 0; rmc_j < rmc_iStringLength; rmc_j++)
        str_destination[rmc_j] = str_source[rmc_j];
    
    str_destination[rmc_j] = '\0';
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

