#include <stdio.h>
#include <stdlib.h> 

int main(void)
{
    // function declarations
    void MyAlloc(int **ptr, unsigned int numberOfElements);

    // variable declarations
    int *piArray_rc = NULL;
    unsigned int num_elements_rc;
    int i_rc;

    // code
    printf("\n\n");
    printf("How many elements you want in integer array? \n\n");
    scanf("%u", &num_elements_rc);

    printf("\n\n");
    MyAlloc(&piArray_rc, num_elements_rc);

    printf("Enter %u elements to fill up your integer array: \n\n", num_elements_rc);
    for (i_rc = 0; i_rc < num_elements_rc; i_rc++)
        scanf("%d", &piArray_rc[i_rc]);

    printf("\n\n");
    printf("Enter %u elements entered by you are: \n\n", num_elements_rc);
    for (i_rc = 0; i_rc < num_elements_rc; i_rc++)
        printf("%u\n", piArray_rc[i_rc]);

    printf("\n\n");
    if (piArray_rc)
    {
        free(piArray_rc);
        piArray_rc = NULL;
        printf("Memory allocated has now been successfully freed!\n\n");
    }

    return(0);
}

void MyAlloc(int **ptr, unsigned int numberOfElements)
{
    // code
    *ptr = (int *)malloc(numberOfElements * sizeof(int));
    if (*ptr == NULL)
    {
        printf("Could not allocate memory! Exiting now..\n\n");
        exit(0);
    }
    printf("MyAlloc() has successfully allocated %u bytes for integer array!\n", numberOfElements * sizeof(int));
}
