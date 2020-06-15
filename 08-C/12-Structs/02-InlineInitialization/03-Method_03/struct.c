#include <stdio.h>

int main(void)
{
    // defining struct
    struct MyData
    {
        int i;
        float f;
        double d;
        char c;
    } data = {4, 5.4f, 62.24323, 'K' }; // inline initialization of struct 


    // displaying values of the data members of struct MyData
    printf("\n\n");
    printf("DATA MEMBERS OF struct MyData ARE: \n\n");
    printf("i = %d\n", data.i);
    printf("f = %f\n", data.f);
    printf("d = %lf\n", data.d);
    printf("c = %c\n", data.c);

    return(0);
}
