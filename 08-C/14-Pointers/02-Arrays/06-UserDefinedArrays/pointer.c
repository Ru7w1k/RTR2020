#include <stdio.h>
#include <stdlib.h>

#define INT_SIZE     sizeof(int)
#define FLOAT_SIZE   sizeof(float)
#define DOUBLE_SIZE  sizeof(double)
#define CHAR_SIZE    sizeof(char)


int main(void)
{
    // variable declarations
    int *ptr_iArray_rc = NULL;
    unsigned int iArrayLength_rc = 0;

    float *ptr_fArray_rc = NULL;
    unsigned int fArrayLength_rc = 0;

    double *ptr_dArray_rc = NULL;
    unsigned int dArrayLength_rc = 0;

    char *ptr_cArray_rc = NULL;
    unsigned int cArrayLength_rc = 0;

    int i_rc;

    // code

    // **** INTEGER ARRAY ****
    printf("\n\n");
    printf("Enter number of elements you want in Integer array: ");
    scanf("%u", &iArrayLength_rc);

    ptr_iArray_rc = (int *)malloc(INT_SIZE * iArrayLength_rc);
    if (ptr_iArray_rc == NULL)
    {
        printf("\n\n");
        printf("MEMORY ALLOCATION FAILED FOR INTEGER ARRAY! Exiting now...");
        exit(0);
    }
    else
    {
        printf("\n\n");
        printf("MEMORY ALLOCATION SUCCEEDED FOR INTEGER ARRAY!");
    }
    
    printf("\n\n");
    printf("Enter The %d integer elements to fill up the integer array: \n\n", iArrayLength_rc);
    for (i_rc = 0; i_rc < iArrayLength_rc; i_rc++)
        scanf("%d", (ptr_iArray_rc + i_rc));


    // **** FLOAT ARRAY ****
    printf("\n\n");
    printf("Enter number of elements you want in Float array: ");
    scanf("%u", &fArrayLength_rc);

    ptr_fArray_rc = (float *)malloc(FLOAT_SIZE * fArrayLength_rc);
    if (ptr_fArray_rc == NULL)
    {
        printf("\n\n");
        printf("MEMORY ALLOCATION FAILED FOR FLOAT ARRAY! Exiting now...");
        exit(0);
    }
    else
    {
        printf("\n\n");
        printf("MEMORY ALLOCATION SUCCEEDED FOR FLOAT ARRAY!");
    }
    
    printf("\n\n");
    printf("Enter The %d float elements to fill up the float array: \n\n", fArrayLength_rc);
    for (i_rc = 0; i_rc < fArrayLength_rc; i_rc++)
        scanf("%f", (ptr_fArray_rc + i_rc));


    // **** DOUBLE ARRAY ****
    printf("\n\n");
    printf("Enter number of elements you want in Double array: ");
    scanf("%u", &dArrayLength_rc);

    ptr_dArray_rc = (double *)malloc(DOUBLE_SIZE * dArrayLength_rc);
    if (ptr_dArray_rc == NULL)
    {
        printf("\n\n");
        printf("MEMORY ALLOCATION FAILED FOR DOUBLE ARRAY! Exiting now...");
        exit(0);
    }
    else
    {
        printf("\n\n");
        printf("MEMORY ALLOCATION SUCCEEDED FOR DOUBLE ARRAY!");
    }
    
    printf("\n\n");
    printf("Enter The %d double elements to fill up the double array: \n\n", dArrayLength_rc);
    for (i_rc = 0; i_rc < dArrayLength_rc; i_rc++)
        scanf("%lf", (ptr_dArray_rc + i_rc));


    // **** CHAR ARRAY ****
    printf("\n\n");
    printf("Enter number of elements you want in Charavter array: ");
    scanf("%u", &cArrayLength_rc);

    ptr_cArray_rc = (char *)malloc(CHAR_SIZE * cArrayLength_rc);
    if (ptr_cArray_rc == NULL)
    {
        printf("\n\n");
        printf("MEMORY ALLOCATION FAILED FOR CHARACTER ARRAY! Exiting now...");
        exit(0);
    }
    else
    {
        printf("\n\n");
        printf("MEMORY ALLOCATION SUCCEEDED FOR CHARACTER ARRAY!");
    }
    
    printf("\n\n");
    printf("Enter The %d character elements to fill up the character array: \n\n", cArrayLength_rc);
    for (i_rc = 0; i_rc < cArrayLength_rc; i_rc++)
    {
        *(ptr_cArray_rc + i_rc) = getch();
        printf("%c\n", *(ptr_cArray_rc + i_rc));
    }

    // DISPLAY OF ARRAYS

    // **** integer array ****
    printf("\n\n");
    printf("The integer array entered by you and consisting of %d elements is as follows: \n\n", iArrayLength_rc);
    for (i_rc = 0; i_rc < iArrayLength_rc; i_rc++)
        printf("%d \t\t At address: %p\n", *(ptr_iArray_rc + i_rc), (ptr_iArray_rc + i_rc));
    
    // **** float array ****
    printf("\n\n");
    printf("The float array entered by you and consisting of %d elements is as follows: \n\n", fArrayLength_rc);
    for (i_rc = 0; i_rc < fArrayLength_rc; i_rc++)
        printf("%f \t\t At address: %p\n", *(ptr_fArray_rc + i_rc), (ptr_fArray_rc + i_rc));

    // **** double array ****
    printf("\n\n");
    printf("The double array entered by you and consisting of %d elements is as follows: \n\n", dArrayLength_rc);
    for (i_rc = 0; i_rc < dArrayLength_rc; i_rc++)
        printf("%lf \t\t At address: %p\n", *(ptr_dArray_rc + i_rc), (ptr_dArray_rc + i_rc));

    // **** character array ****
    printf("\n\n");
    printf("The character array entered by you and consisting of %d elements is as follows: \n\n", cArrayLength_rc);
    for (i_rc = 0; i_rc < cArrayLength_rc; i_rc++)
        printf("%c \t\t At address: %p\n", *(ptr_cArray_rc + i_rc), (ptr_cArray_rc + i_rc));


    // FREE MEMORY OCCUPIED BY ARRAYS
    if (ptr_cArray_rc)
    {
        free(ptr_cArray_rc);
        ptr_cArray_rc = NULL;

        printf("\n\n");
        printf("MEMORY OCCUPIED BY CHARACTER ARRAY HAS BEEN SUCCESSFULLY FREED!!\n\n");
    }

    if (ptr_dArray_rc)
    {
        free(ptr_dArray_rc);
        ptr_dArray_rc = NULL;

        printf("\n\n");
        printf("MEMORY OCCUPIED BY DOUBLE ARRAY HAS BEEN SUCCESSFULLY FREED!!\n\n");
    }

    if (ptr_fArray_rc)
    {
        free(ptr_fArray_rc);
        ptr_fArray_rc = NULL;

        printf("\n\n");
        printf("MEMORY OCCUPIED BY FLOAT ARRAY HAS BEEN SUCCESSFULLY FREED!!\n\n");
    }

    if (ptr_iArray_rc)
    {
        free(ptr_iArray_rc);
        ptr_iArray_rc = NULL;

        printf("\n\n");
        printf("MEMORY OCCUPIED BY INTEGER ARRAY HAS BEEN SUCCESSFULLY FREED!!\n\n");
    }

    return(0);
}