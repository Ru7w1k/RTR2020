#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    // function declarations
    void MultiplyArrayElementsByNumber(int *, int, int);

    // variable declarations
    int *iArray_rc = NULL;
    int num_elements_rc;
    int i_rc, num_rc;

    // code
    printf("\n\n");
    printf("How many elements you want in integer array: ");
    scanf("%d", &num_elements_rc);

    iArray_rc = (int *)malloc(num_elements_rc * sizeof(i_rc));
    if (iArray_rc == NULL)
    {
        printf("MEMORY ALLOCATION FAILED FOR iArray! Exiting now..\n\n");
        exit(0);
    }

    printf("\n\n");
    printf("Enter %d elements for integer array: ", num_elements_rc);
    for (i_rc = 0; i_rc < num_elements_rc; i_rc++)
        scanf("%d", &iArray_rc[i_rc]);

    // ONE
    printf("\n\n");
    printf("Array before passing to function MultiplyArrayElementsByNumber(): \n\n");
    for (i_rc = 0; i_rc < num_elements_rc; i_rc++)
        printf("iArray[%d] = %d\n", i_rc, iArray_rc[i_rc]);

    printf("\n\n");
    printf("Enter number by which you want to multiply each array element: ");
    scanf("%d", &num_rc);

    MultiplyArrayElementsByNumber(iArray_rc, num_elements_rc, num_rc);

    printf("\n\n");
    printf("Array returned from function MultiplyArrayElementsByNumber(): \n\n");
    for (i_rc = 0; i_rc < num_elements_rc; i_rc++)
        printf("iArray[%d] = %d\n", i_rc, iArray_rc[i_rc]);

    if (iArray_rc)
    {
        free(iArray_rc);
        iArray_rc = NULL;
        printf("\n\n");
        printf("MEMORY ALLOCATED TO iArray HAS BEEN FREED!\n\n");
    }

   return(0);

}

void MultiplyArrayElementsByNumber(int *arr, int iNumElements, int n)
{
    // variable declarations
    int i_rc;

    // code
    for (i_rc = 0; i_rc < iNumElements; i_rc++)
    {
        arr[i_rc] *= n;
    }
}



