#include <stdio.h>

// MACRO CONSTANT USED AS ARRAY SIZE IN SUBSCRIPT AND AS ARREY LENGTH.
// HENCE. THIS PROGRAM'S ARRAYS' SIZES CAN BE SIMPLY CHANGED BY CHANGING THESE FOLLOWING 3 GLOBAL MACRO CONSTANT VALUES,
// BEFORE COMPILING, LINKING AND EXECUTING THE PROGRAM!

#define INT_ARRAY_NUM_ELEMENTS 6
#define FLOAT_ARRAY_NUM_ELEMENTS 4
#define CHAR_ARRAY_NUM_ELEMENTS 10

int main(void)
{
    // variable declarations
    int iArray_rmc[INT_ARRAY_NUM_ELEMENTS];
    float fArray_rmc[FLOAT_ARRAY_NUM_ELEMENTS];
    char cArray_rmc[CHAR_ARRAY_NUM_ELEMENTS];
    int rmc_i, rmc_num;

    // code

    // ARRAY ELEMENTS INPUT
    printf("\n\n");
    printf("Enter elements for 'Integer' array: \n");
    for (rmc_i = 0; rmc_i < INT_ARRAY_NUM_ELEMENTS; rmc_i++)
        scanf("%d", &iArray_rmc[rmc_i]);

    printf("\n\n");
    printf("Enter elements for 'Floating-Point' array: \n");
    for (rmc_i = 0; rmc_i < FLOAT_ARRAY_NUM_ELEMENTS; rmc_i++)
        scanf("%f", &fArray_rmc[rmc_i]);

    printf("\n\n");
    printf("Enter elements for 'Char' array: \n");
    for (rmc_i = 0; rmc_i < CHAR_ARRAY_NUM_ELEMENTS; rmc_i++)
    {
        cArray_rmc[rmc_i] = getch();
        printf("%c", cArray_rmc[rmc_i]);
    }
    
 
    // ARRAY ELEMENTS OUTPUT
    printf("\n\n");
    printf("'Integer' array entered by you: \n");
    for (rmc_i = 0; rmc_i < INT_ARRAY_NUM_ELEMENTS; rmc_i++)
        printf("%d\n", iArray_rmc[rmc_i]);

    printf("\n\n");
    printf("'Floating-Point' array entered by you: \n");
    for (rmc_i = 0; rmc_i < FLOAT_ARRAY_NUM_ELEMENTS; rmc_i++)
        printf("%f\n", fArray_rmc[rmc_i]);

    printf("\n\n");
    printf("'Char' array entered by you: \n");
    for (rmc_i = 0; rmc_i < CHAR_ARRAY_NUM_ELEMENTS; rmc_i++)
        printf("%c\n", cArray_rmc[rmc_i]);
    
    printf("\n\n");
    return(0);
}


