#include <stdio.h>

int main(void)
{
    // variable declarations
    int rmc_iArray[] = { 9, 30, 6, 12, 98, 95, 20, 23, 2, 45 };
    int rmc_int_size;
    int rmc_iArray_size;
    int rmc_iArray_num_elements;

    float rmc_fArray[] = { 1.2f, 2.3f, 3.4f, 4.5f, 5.6f, 6.7f, 7.8f, 8.9f };
    int rmc_float_size;
    int rmc_fArray_size;
    int rmc_fArray_num_elements;

    char rmc_cArray[] = { 'A', 'S', 'T', 'R', 'O', 'M', 'E', 'D', 'I', 'C', 'O', 'M', 'P' };
    int rmc_char_size;
    int rmc_cArray_size;
    int rmc_cArray_num_elements;

    int rmc_i;

    // code

    // iArray[]
    printf("\n\n");
    printf("Inline initalization and loop (for) display of elements of array 'iArray[]': \n\n");

    rmc_int_size = sizeof(int);
    rmc_iArray_size = sizeof(rmc_iArray);
    rmc_iArray_num_elements = rmc_iArray_size / rmc_int_size;

    for(rmc_i = 0; rmc_i < rmc_iArray_num_elements; rmc_i++)
    {
        printf("iArray[%d] (Element %d)  = %d\n", rmc_i, rmc_i+1, rmc_iArray[rmc_i]);
    }
    
    printf("\n\n");
    printf("Size of data types 'int'                          = %d bytes\n", rmc_int_size);
    printf("Number of elements in 'int' array 'iArray[]'      = %d elements\n", rmc_iArray_num_elements);
    printf("Size of Array 'iArray[]' (%d elements * %d bytes) = %d bytes\n\n", rmc_iArray_num_elements, rmc_int_size, rmc_iArray_size);


    // fArray[]
    printf("\n\n");
    printf("Inline initalization and loop (while) display of elements of array 'fArray[]': \n\n");

    rmc_float_size = sizeof(float);
    rmc_fArray_size = sizeof(rmc_fArray);
    rmc_fArray_num_elements = rmc_fArray_size / rmc_float_size;

    rmc_i = 0;
    while (rmc_i < rmc_fArray_num_elements)
    {
        printf("fArray[%d] (Element %d)  = %f\n", rmc_i, rmc_i+1, rmc_fArray[rmc_i]);
        rmc_i++;
    }

    printf("Size of data types 'float'                        = %d bytes\n", rmc_float_size);
    printf("Number of elements in 'float' array 'fArray[]'    = %d elements\n", rmc_fArray_num_elements);
    printf("Size of Array 'fArray[]' (%d elements * %d bytes) = %d bytes\n\n", rmc_fArray_num_elements, rmc_float_size, rmc_fArray_size);

    // cArray[]
    printf("\n\n");
    printf("Inline initalization and loop (do-while) display of elements of array 'cArray[]': \n\n");

    rmc_char_size = sizeof(char);
    rmc_cArray_size = sizeof(rmc_cArray);
    rmc_cArray_num_elements = rmc_cArray_size / rmc_char_size;

    rmc_i = 0;
    do
    {
        printf("cArray[%d] (Element %d)   = %c\n", rmc_i, rmc_i+1, rmc_cArray[rmc_i]);
        rmc_i++;
    } while (rmc_i < rmc_cArray_num_elements);

    printf("Size of data types 'char'                         = %d bytes\n", rmc_char_size);
    printf("Number of elements in 'char' array 'cArray[]'     = %d elements\n", rmc_cArray_num_elements);
    printf("Size of Array 'cArray[]' (%d elements * %d bytes) = %d bytes\n\n", rmc_cArray_num_elements, rmc_char_size, rmc_cArray_size);

    return(0);
}


