#include <stdio.h>

// defining struct
struct MyPoint
{
    int x;
    int y;
};


int main(void)
{
    struct MyPoint point_A_rc, point_B_rc, point_C_rc, point_D_rc, point_E_rc; // declaring 5 struct variable of type 'struct MyPoint' locally..
    
    // code
    // user input for the data members of struct MyPoint variable 'point_A'
    printf("\n\n");
    printf("Enter X coordinate for point A:");
    scanf("%d", &point_A_rc.x);
    printf("Enter Y coordinate for point A:");
    scanf("%d", &point_A_rc.y);

    // user input for the data members of struct MyPoint variable 'point_B'
    printf("\n\n");
    printf("Enter X coordinate for point B:");
    scanf("%d", &point_B_rc.x);
    printf("Enter Y coordinate for point B:");
    scanf("%d", &point_B_rc.y);

    // user input for the data members of struct MyPoint variable 'point_C'
    printf("\n\n");
    printf("Enter X coordinate for point C:");
    scanf("%d", &point_C_rc.x);
    printf("Enter Y coordinate for point C:");
    scanf("%d", &point_C_rc.y);

    // user input for the data members of struct MyPoint variable 'point_D'
    printf("\n\n");
    printf("Enter X coordinate for point D:");
    scanf("%d", &point_D_rc.x);
    printf("Enter Y coordinate for point D:");
    scanf("%d", &point_D_rc.y);

    // user input for the data members of struct MyPoint variable 'point_E'
    printf("\n\n");
    printf("Enter X coordinate for point E:");
    scanf("%d", &point_E_rc.x);
    printf("Enter Y coordinate for point E:");
    scanf("%d", &point_E_rc.y);

    // displaying values of the data members of struct MyPoint (all variables)
    printf("\n\n");
    printf("Coordinates (x, y) of point A are: (%d, %d)\n\n", point_A_rc.x, point_A_rc.y);
    printf("Coordinates (x, y) of point B are: (%d, %d)\n\n", point_B_rc.x, point_B_rc.y);
    printf("Coordinates (x, y) of point C are: (%d, %d)\n\n", point_C_rc.x, point_C_rc.y);
    printf("Coordinates (x, y) of point D are: (%d, %d)\n\n", point_D_rc.x, point_D_rc.y);
    printf("Coordinates (x, y) of point E are: (%d, %d)\n\n", point_E_rc.x, point_E_rc.y);

    return(0);
}
