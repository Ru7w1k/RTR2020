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
    // variable declarations

    // 35 will be assigned to 'i' of 'data_one'
    // 3.9 will be assigned to 'f' of 'data_one'
    // 1.12313 will be assigned to 'd' of 'data_one'
    // B will be assigned to 'c' of 'data_one'
    struct MyData data_one_rmc = {35, 3.9f, 1.12313, 'B'}; // inline initialization

    // A will be assigned to 'i' of 'data_two': but 'A' is char so its ASCII value will be assignmed
    // 4.9 will be passigned to 'f' of 'data_two'
    // 541.313 will be assigned to 'd' of 'data_two'
    // 69 will be assigned to 'c' of 'data_two': but 69 is int so it is considered as ASCII value (d)
    struct MyData data_two_rmc = { 'A', 4.9f, 541.313, 69}; // inline initialization

    // 36 will be assigned to 'i' of 'data_three'
    // G will be assigned to 'f' of 'data_three': but G is char so its ASCII value will be assigned
    // 0.00000 will be assigned to 'd' of 'data_three'
    // no character will be assigned to 'c' of 'data_three'
    struct MyData data_three_rmc = {36, 'G'}; // inline initialization

    // 124 will be assigned to 'i' of 'data_four'
    // 0.00000 will be assigned to 'f' of 'data_four'
    // 0.00000 will be assigned to 'd' of 'data_four'
    // no character will be assigned to 'c' of 'data_four'
    struct MyData data_four_rmc = { 124 }; // inline initialization

    // displaying values of the data members of struct MyData
    printf("\n\n");
    printf("DATA MEMBERS OF struct MyData data_one ARE: \n\n");
    printf("i = %d\n", data_one_rmc.i);
    printf("f = %f\n", data_one_rmc.f);
    printf("d = %lf\n", data_one_rmc.d);
    printf("c = %c\n", data_one_rmc.c);

    printf("\n\n");
    printf("DATA MEMBERS OF struct MyData data_two ARE: \n\n");
    printf("i = %d\n", data_two_rmc.i);
    printf("f = %f\n", data_two_rmc.f);
    printf("d = %lf\n", data_two_rmc.d);
    printf("c = %c\n", data_two_rmc.c);

    printf("\n\n");
    printf("DATA MEMBERS OF struct MyData data_three ARE: \n\n");
    printf("i = %d\n", data_three_rmc.i);
    printf("f = %f\n", data_three_rmc.f);
    printf("d = %lf\n", data_three_rmc.d);
    printf("c = %c\n", data_three_rmc.c);

    printf("\n\n");
    printf("DATA MEMBERS OF struct MyData data_four ARE: \n\n");
    printf("i = %d\n", data_four_rmc.i);
    printf("f = %f\n", data_four_rmc.f);
    printf("d = %lf\n", data_four_rmc.d);
    printf("c = %c\n", data_four_rmc.c);

    return(0);
}
