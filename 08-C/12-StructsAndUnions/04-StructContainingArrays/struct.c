#include <stdio.h>
#include <string.h>

#define INT_ARRAY_SIZE   10
#define FLOAT_ARRAY_SIZE 5
#define CHAR_ARRAY_SIZE  26

#define NUM_STRINGS 10
#define MAX_CHARACTERS_PER_STRING 20

#define ALPHABET_BEGINING 65 // 'A'

// defining struct
struct MyDataOne
{
    int iArray[INT_ARRAY_SIZE];
    float fArray[FLOAT_ARRAY_SIZE];
};

struct MyDataTwo
{
    char cArray[CHAR_ARRAY_SIZE];
    char strArray[NUM_STRINGS][MAX_CHARACTERS_PER_STRING];
};


int main(void)
{
    // variable declarations
    struct MyDataOne data_one_rc;
    struct MyDataTwo data_two_rc;
    int i_rc;
    
    // code
    // piece-meal assignment (HARD CODED)
    data_one_rc.fArray[0] = 0.1f;
    data_one_rc.fArray[1] = 1.2f;
    data_one_rc.fArray[2] = 2.3f;
    data_one_rc.fArray[3] = 3.4f;
    data_one_rc.fArray[4] = 4.5f;

    // loop assignment (USER INPUT)
    printf("\n\n");
    printf("Enter %d Integers: \n\n", INT_ARRAY_SIZE);
    for (i_rc = 0; i_rc < INT_ARRAY_SIZE; i_rc++)
        scanf("%d", &data_one_rc.iArray[i_rc]);

    // loop assignment (HARD CODED)
    for (i_rc = 0; i_rc < CHAR_ARRAY_SIZE; i_rc++)
        data_two_rc.cArray[i_rc] = (char)(i_rc + ALPHABET_BEGINING);

    // piece-meal assignment (HARD-CODED)
    strcpy(data_two_rc.strArray[0], "Welcome!!");
    strcpy(data_two_rc.strArray[1], "This");
    strcpy(data_two_rc.strArray[2], "Is");
    strcpy(data_two_rc.strArray[3], "ASTROMEDICOMP");
    strcpy(data_two_rc.strArray[4], "Real");
    strcpy(data_two_rc.strArray[5], "Time");
    strcpy(data_two_rc.strArray[6], "Rendering");
    strcpy(data_two_rc.strArray[7], "Batch");
    strcpy(data_two_rc.strArray[8], "Of");
    strcpy(data_two_rc.strArray[9], "2020-2021!");

    // displaying data members of 'struct DataOne' and their values
    printf("\n\n");
    printf("Members of 'struct DataOne' alongwith thier assigned values are: \n\n");

    printf("\n\n");
    printf("Integer Array (data_one.iArray[]): \n\n");
    for (i_rc = 0; i_rc < INT_ARRAY_SIZE; i_rc++)   
        printf("data_one.iArray[%d] = %d\n", i_rc, data_one_rc.iArray[i_rc]);

    printf("\n\n");
    printf("Float Array (data_one.fArray[]): \n\n");
    for (i_rc = 0; i_rc < FLOAT_ARRAY_SIZE; i_rc++)   
        printf("data_one.fArray[%d] = %f\n", i_rc, data_one_rc.fArray[i_rc]);

    // displaying data members of 'struct DataTwo' and their values
    printf("\n\n");
    printf("Members of 'struct DataTwo' alongwith thier assigned values are: \n\n");

    printf("\n\n");
    printf("Character Array (data_two.cArray[]): \n\n");
    for (i_rc = 0; i_rc < CHAR_ARRAY_SIZE; i_rc++)   
        printf("data_two.cArray[%d] = %c\n", i_rc, data_two_rc.cArray[i_rc]);

    printf("\n\n");
    printf("String Array (data_one.strArray[]): \n\n");
    for (i_rc = 0; i_rc < NUM_STRINGS; i_rc++)   
        printf("data_two.strArray[%d] = %s\n", i_rc, data_two_rc.strArray[i_rc]);

    printf("\n\n");

    return(0);
}
