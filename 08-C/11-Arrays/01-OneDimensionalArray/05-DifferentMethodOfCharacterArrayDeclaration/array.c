#include <stdio.h>

int main(void)
{
    // variable declarations
    char chArray_01_rmc[] = { 'A', 'S', 'T', 'R', 'O', 'M', 'E', 'D', 'I', 'C', 'O', 'M', 'P', '\0' }; // MUST GIVE '\0' FOR PROPER INITIALIZATION
    char chArray_02_rmc[9] = {'W', 'E', 'L', 'C', 'O', 'M', 'E', 'S', '\0' }; // MUST GIVE '\0' FOR PROPER INITIALIZATION
    char chArray_03_rmc[] = { 'Y', 'O', 'U', '\0' }; // MUST GIVE '\0' FOR PROPER INITIALIZATION
    char chArray_04_rmc[] = "To"; // '\0' IS ASSUMED, SIZE IS GIVEN AS 3, ALTHOUGH ONLY 2 CHARACTERS ARE PRESENT
    char chArray_05_rmc[] = "REAL TIME RENDERING BATCH OF 2020-21"; // '\0' IS ASSUMED, SIZE IS GIVEN AS 37, ALTHOUGH ONLY 36 CHARACTERS ARE PRESENT

    char chArray_withoutNullTerminator_rmc[] = {'H', 'e', 'l', 'l', 'o'};

    // code
    printf("\n\n");
    
    printf("Size of chArray_01: %lu \n\n", sizeof(chArray_01_rmc));
    printf("Size of chArray_02: %lu \n\n", sizeof(chArray_02_rmc));
    printf("Size of chArray_03: %lu \n\n", sizeof(chArray_03_rmc));
    printf("Size of chArray_04: %lu \n\n", sizeof(chArray_04_rmc));
    printf("Size of chArray_05: %lu \n\n", sizeof(chArray_05_rmc));

    printf("\n\n");

    printf("The strings are: \n\n");
    printf("chArray_01: %s \n\n", chArray_01_rmc);
    printf("chArray_02: %s \n\n", chArray_02_rmc);
    printf("chArray_03: %s \n\n", chArray_03_rmc);
    printf("chArray_04: %s \n\n", chArray_04_rmc);
    printf("chArray_05: %s \n\n", chArray_05_rmc);

    printf("\n\n");
    printf("Size of chArray_WithoutNullTerminator: %lu\n\n", sizeof(chArray_withoutNullTerminator_rmc));
    printf("chArray_WithoutNullTerminator: %s\n\n", chArray_withoutNullTerminator_rmc); // will display garbage value at the end of string due to absence of \0

    return(0);
}


