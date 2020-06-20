#include <stdio.h>

// defining struct
struct MyData
{
    int i;
    float f;
    double d;
    char c;
};

int main(void)
{
    // variable declaration
    struct MyData data_rc;

    // code
    // assigning data values to the data members of struct MyData
    data_rc.i = 25;
    data_rc.f = 12.35f;
    data_rc.d = 1.123123;
    data_rc.c = 'A';

    // displaying values of the data members of struct MyData
    printf("\n\n");
    printf("DATA MEMBERS OF struct MyData ARE: \n\n");
    printf("i = %d\n", data_rc.i);
    printf("f = %f\n", data_rc.f);
    printf("d = %lf\n", data_rc.d);
    printf("c = %c\n", data_rc.c);

    printf("\n\n");
    printf("ADDRESSES OF DATA MEMBERS OF struct MyData ARE: \n\n");
    printf("'i' occupies addresses from %p\n", &data_rc.i);
    printf("'f' occupies addresses from %p\n", &data_rc.f);
    printf("'d' occupies addresses from %p\n", &data_rc.d);
    printf("'c' occupies address %p\n", &data_rc.c);

    printf("Starting address of struct MyData varialbe 'data' = %p\n\n", &data_rc);

    return(0);
}
