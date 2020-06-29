#include <stdio.h>
#include <stdlib.h> // contains prototypes of malloc() and free()

int main(void)
{
    // variable declarations
    int *ptr_iArray_rc = NULL;
    unsigned int iArrayLength_rc = 0;

    int i_rc;

    // code
    printf("\n\n");
    printf("Enter number of elements you want in Integer array: ");
    scanf("%u", &iArrayLength_rc);

    ptr_iArray_rc = (int *)malloc(sizeof(int) * iArrayLength_rc);
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
        printf("MEMORY ADDRESSES FROM %p to %p HAVE BEEN ALLOCATED TO INTEGER ARRAY!\n\n", ptr_iArray_rc, (ptr_iArray_rc + (iArrayLength_rc - 1)));
    }
    
    printf("\n\n");
    printf("Enter The %d integer elements to fill up the integer array: \n\n", iArrayLength_rc);
    for (i_rc = 0; i_rc < iArrayLength_rc; i_rc++)
        scanf("%d", (ptr_iArray_rc + i_rc));


    printf("\n\n");
    printf("The integer array entered by you and consisting of %d elements is as follows: \n\n", iArrayLength_rc);

    printf("The integer array entered by you and consisting of %d elements is as follows: \n\n", iArrayLength_rc);
    for (i_rc = 0; i_rc < iArrayLength_rc; i_rc++)
        printf("ptr_iArray[%d] = %d \t\t At address &ptr_iArray[%d]: %p\n", i_rc, ptr_iArray_rc[i_rc], i_rc, &ptr_iArray_rc[i_rc]);

    printf("\n\n");
    for (i_rc = 0; i_rc < iArrayLength_rc; i_rc++)
        printf("*(ptr_iArray + %d) = %d \t\t At address (ptr_iArray + %d): %p\n", i_rc, *(ptr_iArray_rc + i_rc), i_rc, (ptr_iArray_rc + i_rc));
    

    // FREE MEMORY OCCUPIED BY ARRAYS
    if (ptr_iArray_rc)
    {
        free(ptr_iArray_rc);
        ptr_iArray_rc = NULL;

        printf("\n\n");
        printf("MEMORY OCCUPIED BY INTEGER ARRAY HAS BEEN SUCCESSFULLY FREED!!\n\n");
    }

    return(0);
}
