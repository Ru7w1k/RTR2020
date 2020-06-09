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

    // code

    // iArray[]
    printf("\n\n");
    printf("Inline initalization and piece-meal display of elements of array 'iArray[]': \n\n");
    printf("iArray[0] (1st element)  = %d\n", rmc_iArray[0]);
    printf("iArray[1] (2nd element)  = %d\n", rmc_iArray[1]);
    printf("iArray[2] (4rd element)  = %d\n", rmc_iArray[2]);
    printf("iArray[3] (4th element)  = %d\n", rmc_iArray[3]);
    printf("iArray[4] (5th element)  = %d\n", rmc_iArray[4]);
    printf("iArray[5] (6th element)  = %d\n", rmc_iArray[5]);
    printf("iArray[6] (7th element)  = %d\n", rmc_iArray[6]);
    printf("iArray[7] (8th element)  = %d\n", rmc_iArray[7]);
    printf("iArray[8] (9th element)  = %d\n", rmc_iArray[8]);
    printf("iArray[9] (10th element) = %d\n", rmc_iArray[9]);

    rmc_int_size = sizeof(int);
    rmc_iArray_size = sizeof(rmc_iArray);
    rmc_iArray_num_elements = rmc_iArray_size / rmc_int_size;
    printf("Size of data types 'int'                          = %d bytes\n", rmc_int_size);
    printf("Number of elements in 'int' array 'iArray[]'      = %d elements\n", rmc_iArray_num_elements);
    printf("Size of Array 'iArray[]' (%d elements * %d bytes) = %d bytes\n\n", rmc_iArray_num_elements, rmc_int_size, rmc_iArray_size);


    // fArray[]
    printf("\n\n");
    printf("Inline initalization and piece-meal display of elements of array 'fArray[]': \n\n");
    printf("fArray[0] (1st element)  = %f\n", rmc_fArray[0]);
    printf("fArray[1] (2nd element)  = %f\n", rmc_fArray[1]);
    printf("fArray[2] (4rd element)  = %f\n", rmc_fArray[2]);
    printf("fArray[3] (4th element)  = %f\n", rmc_fArray[3]);
    printf("fArray[4] (5th element)  = %f\n", rmc_fArray[4]);
    printf("fArray[5] (6th element)  = %f\n", rmc_fArray[5]);
    printf("fArray[6] (7th element)  = %f\n", rmc_fArray[6]);
    printf("fArray[7] (8th element)  = %f\n", rmc_fArray[7]);
    // printf("fArray[8] (9th element)  = %f\n", rmc_fArray[8]);
    // printf("fArray[9] (10th element) = %f\n", rmc_fArray[9]);

    rmc_float_size = sizeof(float);
    rmc_fArray_size = sizeof(rmc_fArray);
    rmc_fArray_num_elements = rmc_fArray_size / rmc_float_size;
    printf("Size of data types 'float'                        = %d bytes\n", rmc_float_size);
    printf("Number of elements in 'float' array 'fArray[]'    = %d elements\n", rmc_fArray_num_elements);
    printf("Size of Array 'fArray[]' (%d elements * %d bytes) = %d bytes\n\n", rmc_fArray_num_elements, rmc_float_size, rmc_fArray_size);

    // cArray[]
    printf("\n\n");
    printf("Inline initalization and piece-meal display of elements of array 'cArray[]': \n\n");
    printf("cArray[0] (1st element)   = %c\n", rmc_cArray[0]);
    printf("cArray[1] (2nd element)   = %c\n", rmc_cArray[1]);
    printf("cArray[2] (4rd element)   = %c\n", rmc_cArray[2]);
    printf("cArray[3] (4th element)   = %c\n", rmc_cArray[3]);
    printf("cArray[4] (5th element)   = %c\n", rmc_cArray[4]);
    printf("cArray[5] (6th element)   = %c\n", rmc_cArray[5]);
    printf("cArray[6] (7th element)   = %c\n", rmc_cArray[6]);
    printf("cArray[7] (8th element)   = %c\n", rmc_cArray[7]);
    printf("cArray[8] (9th element)   = %c\n", rmc_cArray[8]);
    printf("cArray[9] (10th element)  = %c\n", rmc_cArray[9]);
    printf("cArray[10] (11th element) = %c\n", rmc_cArray[10]);
    printf("cArray[11] (12th element) = %c\n", rmc_cArray[11]);
    printf("cArray[12] (13th element) = %c\n", rmc_cArray[12]);

    rmc_char_size = sizeof(char);
    rmc_cArray_size = sizeof(rmc_cArray);
    rmc_cArray_num_elements = rmc_cArray_size / rmc_char_size;
    printf("Size of data types 'char'                         = %d bytes\n", rmc_char_size);
    printf("Number of elements in 'char' array 'cArray[]'     = %d elements\n", rmc_cArray_num_elements);
    printf("Size of Array 'cArray[]' (%d elements * %d bytes) = %d bytes\n\n", rmc_cArray_num_elements, rmc_char_size, rmc_cArray_size);

    return(0);
}


