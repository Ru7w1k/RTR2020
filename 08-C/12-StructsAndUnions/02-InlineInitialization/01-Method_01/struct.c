#include <stdio.h>

// defining struct
struct MyData
{
    int i;
    float f;
    double d;
    char c;
} data = {40, 1.4f, 12.24323, 'R' }; // inline initialization of struct 


int main(void)
{
    // displaying values of the data members of struct MyData
    printf("\n\n");
    printf("DATA MEMBERS OF struct MyData ARE: \n\n");
    printf("i = %d\n", data.i);
    printf("f = %f\n", data.f);
    printf("d = %lf\n", data.d);
    printf("c = %c\n", data.c);

    return(0);
}
