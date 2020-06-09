#include <stdio.h>

int main(void)
{
    // variable declarations
    int rmc_iArrayOne[10];
    int rmc_iArrayTwo[20];

    // code

    // iArrayOne[]
    rmc_iArrayOne[0] = 4;
    rmc_iArrayOne[1] = 8;
    rmc_iArrayOne[2] = 12;
    rmc_iArrayOne[3] = 16;
    rmc_iArrayOne[4] = 20;
    rmc_iArrayOne[5] = 24;
    rmc_iArrayOne[6] = 28;
    rmc_iArrayOne[7] = 32;
    rmc_iArrayOne[8] = 36;
    rmc_iArrayOne[9] = 40;

    printf("\n\n");
    printf("Piece-Meal (hard-coded) Assignment and display of elements to array 'iArrayOne[]': \n\n");
    printf("1st  element of array 'iArrayOne[]' or element at 0th index of array 'iArrayOne[]' = %d\n", rmc_iArrayOne[0]);
    printf("2nd  element of array 'iArrayOne[]' or element at 1st index of array 'iArrayOne[]' = %d\n", rmc_iArrayOne[1]);
    printf("3rd  element of array 'iArrayOne[]' or element at 2nd index of array 'iArrayOne[]' = %d\n", rmc_iArrayOne[2]);
    printf("4th  element of array 'iArrayOne[]' or element at 3rd index of array 'iArrayOne[]' = %d\n", rmc_iArrayOne[3]);
    printf("5th  element of array 'iArrayOne[]' or element at 4th index of array 'iArrayOne[]' = %d\n", rmc_iArrayOne[4]);
    printf("6th  element of array 'iArrayOne[]' or element at 5th index of array 'iArrayOne[]' = %d\n", rmc_iArrayOne[5]);
    printf("7th  element of array 'iArrayOne[]' or element at 6th index of array 'iArrayOne[]' = %d\n", rmc_iArrayOne[6]);
    printf("8th  element of array 'iArrayOne[]' or element at 7th index of array 'iArrayOne[]' = %d\n", rmc_iArrayOne[7]);
    printf("9th  element of array 'iArrayOne[]' or element at 8th index of array 'iArrayOne[]' = %d\n", rmc_iArrayOne[8]);
    printf("10th element of array 'iArrayOne[]' or element at 9th index of array 'iArrayOne[]' = %d\n", rmc_iArrayOne[9]);

    // iArrayTwo[]

    printf("\n\n");
    printf("Enter  1st element of array 'iArrayTwo[]': ");
    scanf("%d", &rmc_iArrayTwo[0]);
    printf("Enter  2nd element of array 'iArrayTwo[]': ");
    scanf("%d", &rmc_iArrayTwo[1]);
    printf("Enter  3rd element of array 'iArrayTwo[]': ");
    scanf("%d", &rmc_iArrayTwo[2]);
    printf("Enter  4th element of array 'iArrayTwo[]': ");
    scanf("%d", &rmc_iArrayTwo[3]);
    printf("Enter  5th element of array 'iArrayTwo[]': ");
    scanf("%d", &rmc_iArrayTwo[4]);
    printf("Enter  6th element of array 'iArrayTwo[]': ");
    scanf("%d", &rmc_iArrayTwo[5]);
    printf("Enter  7th element of array 'iArrayTwo[]': ");
    scanf("%d", &rmc_iArrayTwo[6]);
    printf("Enter  8th element of array 'iArrayTwo[]': ");
    scanf("%d", &rmc_iArrayTwo[7]);
    printf("Enter  9th element of array 'iArrayTwo[]': ");
    scanf("%d", &rmc_iArrayTwo[8]);
    printf("Enter 10th element of array 'iArrayTwo[]': ");
    scanf("%d", &rmc_iArrayTwo[9]);

    printf("\n\n");
    printf("Piece-Meal (User Input) Assignment and display of elements to array 'iArrayTwo[]': \n\n");
    printf("1st  element of array 'iArrayTwo[]' or element at 0th index of array 'iArrayTwo[]' = %d\n", rmc_iArrayTwo[0]);
    printf("2nd  element of array 'iArrayTwo[]' or element at 1st index of array 'iArrayTwo[]' = %d\n", rmc_iArrayTwo[1]);
    printf("3rd  element of array 'iArrayTwo[]' or element at 2nd index of array 'iArrayTwo[]' = %d\n", rmc_iArrayTwo[2]);
    printf("4th  element of array 'iArrayTwo[]' or element at 3rd index of array 'iArrayTwo[]' = %d\n", rmc_iArrayTwo[3]);
    printf("5th  element of array 'iArrayTwo[]' or element at 4th index of array 'iArrayTwo[]' = %d\n", rmc_iArrayTwo[4]);
    printf("6th  element of array 'iArrayTwo[]' or element at 5th index of array 'iArrayTwo[]' = %d\n", rmc_iArrayTwo[5]);
    printf("7th  element of array 'iArrayTwo[]' or element at 6th index of array 'iArrayTwo[]' = %d\n", rmc_iArrayTwo[6]);
    printf("8th  element of array 'iArrayTwo[]' or element at 7th index of array 'iArrayTwo[]' = %d\n", rmc_iArrayTwo[7]);
    printf("9th  element of array 'iArrayTwo[]' or element at 8th index of array 'iArrayTwo[]' = %d\n", rmc_iArrayTwo[8]);
    printf("10th element of array 'iArrayTwo[]' or element at 9th index of array 'iArrayTwo[]' = %d\n", rmc_iArrayTwo[9]);
    
    return(0);
}


