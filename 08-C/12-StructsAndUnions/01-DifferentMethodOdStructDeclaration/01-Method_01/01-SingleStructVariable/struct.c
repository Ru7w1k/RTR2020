#include <stdio.h>

// defining struct
struct MyData
{
    int i;
    float f;
    double d;
    char c;
} data; // declaring a single struct variable of type 'struct MyData' globally..

int main(void)
{
    // variable declaration
    int i_rmc_size;
    int f_rmc_size;
    int d_rmc_size;
    int struct_MyData_rmc_size;

    // code
    // assigning data values to the data members of struct MyData
    data.i = 25;
    data.f = 12.35f;
    data.d = 1.123123;

    // displaying values of the data members of struct MyData
    printf("\n\n");
    printf("DATA MEMBERS OF struct MyData ARE: \n\n");
    printf("i = %d\n", data.i);
    printf("f = %f\n", data.f);
    printf("d = %lf\n", data.d);

    // calculating sizes (in bytes) of data members of struct MyData
    i_rmc_size = sizeof(data.i);
    f_rmc_size = sizeof(data.f);
    d_rmc_size = sizeof(data.d);

    // Displaying sizes (in bytes) of data members of struct MyData
    printf("\n\n");
    printf("SIZES (in bytes) OF DATA MEMBERS OF struct MyData ARE: \n\n");
    printf("Size of i = %d bytes \n", i_rmc_size);
    printf("Size of f = %d bytes \n", f_rmc_size);
    printf("Size of d = %d bytes \n", d_rmc_size);

    // calculating size (in bytes) of the entire struct MyData
    struct_MyData_rmc_size = sizeof(struct MyData);

    // displaying size (in bytes) of the entire struct MyData
    printf("\n\n");
    printf("Size of struct MyData: %d bytes\n\n", struct_MyData_rmc_size);

    return(0);
}
