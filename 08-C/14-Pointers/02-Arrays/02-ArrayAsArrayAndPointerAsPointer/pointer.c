#include <stdio.h>

int main(void)
{
    // variable declarations
    int iArray_rc[] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 }; // integer array
    int *ptr_iArray_rc = NULL; // integer pointer

    // code

    // using array name as array, 
    // value of xth element of array: array[x]
    // address of xth element of array: &array[x]
    printf("\n\n");
    printf("-- Using array name as array, value of xth element of array: array[x] and address of xth element of array: &array[x]\n\n");
    printf("Integer array elements and their addresses: \n\n");
    printf("iArray[0] = %5d \t at address &iArray[0]: %p\n", iArray_rc[0], &iArray_rc[0]);
    printf("iArray[1] = %5d \t at address &iArray[1]: %p\n", iArray_rc[1], &iArray_rc[1]);
    printf("iArray[2] = %5d \t at address &iArray[2]: %p\n", iArray_rc[2], &iArray_rc[2]);
    printf("iArray[3] = %5d \t at address &iArray[3]: %p\n", iArray_rc[3], &iArray_rc[3]);
    printf("iArray[4] = %5d \t at address &iArray[4]: %p\n", iArray_rc[4], &iArray_rc[4]);
    printf("iArray[5] = %5d \t at address &iArray[5]: %p\n", iArray_rc[5], &iArray_rc[5]);
    printf("iArray[6] = %5d \t at address &iArray[6]: %p\n", iArray_rc[6], &iArray_rc[6]);
    printf("iArray[7] = %5d \t at address &iArray[7]: %p\n", iArray_rc[7], &iArray_rc[7]);
    printf("iArray[8] = %5d \t at address &iArray[8]: %p\n", iArray_rc[8], &iArray_rc[8]);
    printf("iArray[9] = %5d \t at address &iArray[9]: %p\n", iArray_rc[9], &iArray_rc[9]);

    // ASSIGNING BASE ADDRESS OF INTEGER ARRAY iArray TO INTEGER POINTER ptr_iArray
    ptr_iArray_rc = iArray_rc;
    
    // using pointer as pointer
    // value of xth element of array: *(ptr_iArray + x)
    // address of xth element of array: (ptr_iArray + x)
    printf("\n\n");
    printf("-- Using pointer as pointer, value of xth element of array: *(ptr_iArray + x) and address of xth element of array: (ptr_iArray + x)\n\n");
    printf("Integer array elements and their addresses: \n\n");
    printf("*(ptr_iArray + 0) = %5d \t at address (ptr_iArray + 0): %p\n", *(ptr_iArray_rc + 0), (ptr_iArray_rc + 0));
    printf("*(ptr_iArray + 1) = %5d \t at address (ptr_iArray + 1): %p\n", *(ptr_iArray_rc + 1), (ptr_iArray_rc + 1));
    printf("*(ptr_iArray + 2) = %5d \t at address (ptr_iArray + 2): %p\n", *(ptr_iArray_rc + 2), (ptr_iArray_rc + 2));
    printf("*(ptr_iArray + 3) = %5d \t at address (ptr_iArray + 3): %p\n", *(ptr_iArray_rc + 3), (ptr_iArray_rc + 3));
    printf("*(ptr_iArray + 4) = %5d \t at address (ptr_iArray + 4): %p\n", *(ptr_iArray_rc + 4), (ptr_iArray_rc + 4));
    printf("*(ptr_iArray + 5) = %5d \t at address (ptr_iArray + 5): %p\n", *(ptr_iArray_rc + 5), (ptr_iArray_rc + 5));
    printf("*(ptr_iArray + 6) = %5d \t at address (ptr_iArray + 6): %p\n", *(ptr_iArray_rc + 6), (ptr_iArray_rc + 6));
    printf("*(ptr_iArray + 7) = %5d \t at address (ptr_iArray + 7): %p\n", *(ptr_iArray_rc + 7), (ptr_iArray_rc + 7));
    printf("*(ptr_iArray + 8) = %5d \t at address (ptr_iArray + 8): %p\n", *(ptr_iArray_rc + 8), (ptr_iArray_rc + 8));
    printf("*(ptr_iArray + 9) = %5d \t at address (ptr_iArray + 9): %p\n", *(ptr_iArray_rc + 9), (ptr_iArray_rc + 9));

    return(0);
}