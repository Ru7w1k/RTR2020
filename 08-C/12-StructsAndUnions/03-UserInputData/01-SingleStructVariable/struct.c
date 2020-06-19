#include <stdio.h>
#include <conio.h>

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
    struct MyData data_rmc;

    // code
    // user input for values of data members of struct MyData
    printf("\n\n");

    printf("Enter integer value for data member 'i' of struct MyData: \n");
    scanf("%d", &data_rmc.i);

    printf("Enter floating-point value for data member 'f' of struct MyData: \n");
    scanf("%f", &data_rmc.f);

    printf("Enter double value for data member 'd' of struct MyData: \n");
    scanf("%lf", &data_rmc.d);

    printf("Enter character value for data member 'c' of struct MyData: \n");
    data_rmc.c = getch();

    // displaying values of the data members of struct MyData
    printf("\n\n");
    printf("DATA MEMBERS OF struct MyData ARE: \n\n");
    printf("i = %d\n", data_rmc.i);
    printf("f = %f\n", data_rmc.f);
    printf("d = %lf\n", data_rmc.d);
    printf("c = %c\n", data_rmc.c);

    return(0);
}
