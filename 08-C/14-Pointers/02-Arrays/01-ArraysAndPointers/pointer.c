#include <stdio.h>

int main(void)
{
    // variable declarations
    int iArray_rc[] = { 12, 24, 36, 48, 60, 72, 84, 96, 108, 120 };
    float fArray_rc[] = { 1.2f, 2.3f, 3.4f, 5.4f, 1.3f };
    double dArray_rc[] = { 1.2222222, 2.3333333, 3.4444444 };
    char cArray_rc[] = { 'A', 'S', 'T', 'R', 'O', 'M', 'E', 'D', 'I', 'C', 'O', 'M', 'P', '\0' };

    // code
    printf("\n\n");
    printf("Integer array elements and the addresses they occupy are as follows: \n\n");
    printf("iArray[0] = %5d \t at address: %p\n", *(iArray_rc + 0), (iArray_rc + 0));
    printf("iArray[1] = %5d \t at address: %p\n", *(iArray_rc + 1), (iArray_rc + 1));
    printf("iArray[2] = %5d \t at address: %p\n", *(iArray_rc + 2), (iArray_rc + 2));
    printf("iArray[3] = %5d \t at address: %p\n", *(iArray_rc + 3), (iArray_rc + 3));
    printf("iArray[4] = %5d \t at address: %p\n", *(iArray_rc + 4), (iArray_rc + 4));
    printf("iArray[5] = %5d \t at address: %p\n", *(iArray_rc + 5), (iArray_rc + 5));
    printf("iArray[6] = %5d \t at address: %p\n", *(iArray_rc + 6), (iArray_rc + 6));
    printf("iArray[7] = %5d \t at address: %p\n", *(iArray_rc + 7), (iArray_rc + 7));
    printf("iArray[8] = %5d \t at address: %p\n", *(iArray_rc + 8), (iArray_rc + 8));
    printf("iArray[9] = %5d \t at address: %p\n", *(iArray_rc + 9), (iArray_rc + 9));

    printf("\n\n");
    printf("Float array elements and the addresses they occupy are as follows: \n\n");
    printf("fArray[0] = %f \t at address: %p\n", *(fArray_rc + 0), (fArray_rc + 0));
    printf("fArray[1] = %f \t at address: %p\n", *(fArray_rc + 1), (fArray_rc + 1));
    printf("fArray[2] = %f \t at address: %p\n", *(fArray_rc + 2), (fArray_rc + 2));
    printf("fArray[3] = %f \t at address: %p\n", *(fArray_rc + 3), (fArray_rc + 3));
    printf("fArray[4] = %f \t at address: %p\n", *(fArray_rc + 4), (fArray_rc + 4));

    printf("\n\n");
    printf("Double array elements and the addresses they occupy are as follows: \n\n");
    printf("dArray[0] = %lf \t at address: %p\n", *(dArray_rc + 0), (dArray_rc + 0));
    printf("dArray[1] = %lf \t at address: %p\n", *(dArray_rc + 1), (dArray_rc + 1));
    printf("dArray[2] = %lf \t at address: %p\n", *(dArray_rc + 2), (dArray_rc + 2));

    printf("\n\n");
    printf("Character array elements and the addresses they occupy are as follows: \n\n");
    printf("cArray[0]  = %c \t at address: %p\n", *(cArray_rc + 0),  (cArray_rc + 0));
    printf("cArray[1]  = %c \t at address: %p\n", *(cArray_rc + 1),  (cArray_rc + 1));
    printf("cArray[2]  = %c \t at address: %p\n", *(cArray_rc + 2),  (cArray_rc + 2));
    printf("cArray[3]  = %c \t at address: %p\n", *(cArray_rc + 3),  (cArray_rc + 3));
    printf("cArray[4]  = %c \t at address: %p\n", *(cArray_rc + 4),  (cArray_rc + 4));
    printf("cArray[5]  = %c \t at address: %p\n", *(cArray_rc + 5),  (cArray_rc + 5));
    printf("cArray[6]  = %c \t at address: %p\n", *(cArray_rc + 6),  (cArray_rc + 6));
    printf("cArray[7]  = %c \t at address: %p\n", *(cArray_rc + 7),  (cArray_rc + 7));
    printf("cArray[8]  = %c \t at address: %p\n", *(cArray_rc + 8),  (cArray_rc + 8));
    printf("cArray[9]  = %c \t at address: %p\n", *(cArray_rc + 9),  (cArray_rc + 9));
    printf("cArray[10] = %c \t at address: %p\n", *(cArray_rc + 10), (cArray_rc + 10));
    printf("cArray[11] = %c \t at address: %p\n", *(cArray_rc + 11), (cArray_rc + 11));
    printf("cArray[12] = %c \t at address: %p\n", *(cArray_rc + 12), (cArray_rc + 12));
    printf("cArray[13] = %c \t at address: %p\n", *(cArray_rc + 13), (cArray_rc + 13));

    return(0);
}