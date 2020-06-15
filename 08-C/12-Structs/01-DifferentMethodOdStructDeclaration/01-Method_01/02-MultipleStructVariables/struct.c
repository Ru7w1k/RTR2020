#include <stdio.h>

// defining struct
struct MyPoint
{
    int x;
    int y;
} point_A_rc, point_B_rc, point_C_rc, point_D_rc, point_E_rc; // declaring 5 struct variable of type 'struct MyPoint' globally..

int main(void)
{
    // code
    // assigning data values to the data members of struct MyPoint variable 'point_A'
    point_A_rc.x = 4;
    point_A_rc.y = 0;

    // assigning data values to the data members of struct MyPoint variable 'point_B'
    point_B_rc.x = 3;
    point_B_rc.y = 2;

    // assigning data values to the data members of struct MyPoint variable 'point_C'
    point_C_rc.x = 1;
    point_C_rc.y = 4;

    // assigning data values to the data members of struct MyPoint variable 'point_D'
    point_D_rc.x = 5;
    point_D_rc.y = 2;

    // assigning data values to the data members of struct MyPoint variable 'point_E'
    point_E_rc.x = 11;
    point_E_rc.y = 8;

    // displaying values of the data members of struct MyPoint (all variables)
    printf("\n\n");
    printf("Coordinates (x, y) of point A are: (%d, %d)\n\n", point_A_rc.x, point_A_rc.y);
    printf("Coordinates (x, y) of point B are: (%d, %d)\n\n", point_B_rc.x, point_B_rc.y);
    printf("Coordinates (x, y) of point C are: (%d, %d)\n\n", point_C_rc.x, point_C_rc.y);
    printf("Coordinates (x, y) of point D are: (%d, %d)\n\n", point_D_rc.x, point_D_rc.y);
    printf("Coordinates (x, y) of point E are: (%d, %d)\n\n", point_E_rc.x, point_E_rc.y);

    return(0);
}
