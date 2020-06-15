#include <stdio.h>

// defining struct
struct MyData
{
    int i;
    float f;
    double d;
    char c;
};

struct MyData data = {30, 1.413f, 235.234323, 'C' }; // inline initialization of struct 


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
