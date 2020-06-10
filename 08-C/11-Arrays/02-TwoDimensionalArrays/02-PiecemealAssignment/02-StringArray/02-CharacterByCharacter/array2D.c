#include <stdio.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
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
    // ROW 1 / STRING 1
    rmc_strArray[0][0] = 'M';
    rmc_strArray[0][1] = 'y';
    rmc_strArray[0][2] = '\0'; // NULL TERMINATING CHARACTER

    // ROW 2 / STRING 2
    rmc_strArray[1][0] = 'N';
    rmc_strArray[1][1] = 'a';
    rmc_strArray[1][2] = 'm';
    rmc_strArray[1][3] = 'e';
    rmc_strArray[1][4] = '\0'; // NULL TERMINATING CHARACTER

    // ROW 3 / STRING 3
    rmc_strArray[2][0] = 'I';
    rmc_strArray[2][1] = 's';
    rmc_strArray[2][2] = '\0'; // NULL TERMINATING CHARACTER

    // ROW 4 / STRING 4
    rmc_strArray[3][0] = 'R';
    rmc_strArray[3][1] = 'u';
    rmc_strArray[3][2] = 't';
    rmc_strArray[3][3] = 'w';
    rmc_strArray[3][4] = 'i';
    rmc_strArray[3][5] = 'k';
    rmc_strArray[3][6] = '\0'; // NULL TERMINATING CHARACTER

    // ROW 5 / STRING 5
    rmc_strArray[4][0] = 'C';
    rmc_strArray[4][1] = 'h';
    rmc_strArray[4][2] = 'o';
    rmc_strArray[4][3] = 'u';
    rmc_strArray[4][4] = 'g';
    rmc_strArray[4][5] = 'h';
    rmc_strArray[4][6] = 'u';
    rmc_strArray[4][7] = 'l';
    rmc_strArray[4][8] = 'e';
    rmc_strArray[4][9] = '\0'; // NULL TERMINATING CHARACTER

    printf("\n\n");
    printf("Strings in the 2D array: \n\n");

    for (rmc_i = 0; rmc_i < rmc_strArray_num_rows; rmc_i++)
        printf("%s ", rmc_strArray[rmc_i]);

    return(0);
}
