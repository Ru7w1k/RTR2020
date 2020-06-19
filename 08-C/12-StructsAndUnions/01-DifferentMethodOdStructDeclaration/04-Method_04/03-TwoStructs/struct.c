#include <stdio.h>
#include <string.h>

// defining struct
struct MyPoint
{
    int x;
    int y;
}; 


// defining struct
struct MyPointProperties
{
    int quadrant;
    char axis_location[10];
}; 

int main(void)
{

    struct MyPoint point_rc; // declaring a single variable of type 'struct MyPoint' locally..

    struct MyPointProperties point_properties_rc; // declaring a single variable of type 'struct MyPointProperties' locally..

    // code
    // user input for the data members of struct MyPoint variable point
    printf("\n\n");
    printf("Enter X-coordinate for point A: ");
    scanf("%d", &point_rc.x);
    printf("Enter Y-coordinate for point B: ");
    scanf("%d", &point_rc.y);
    
    printf("\n\n");
    printf("Point co-ordinates (x, y) are:  (%d, %d)!\n\n", point_rc.x, point_rc.y);

    if (point_rc.x == 0 && point_rc.y == 0)
        printf("The point is the origin (%d, %d)!\n", point_rc.x, point_rc.y);
    else // either x or y or both are non-zero
    {
        if (point_rc.x == 0) // if X is ZERO, Y is NON-ZERO
        {
            if (point_rc.y < 0) // if Y is -VE
                strcpy(point_properties_rc.axis_location, "Negative Y");

            if (point_rc.y > 0) // if Y is +VE
                strcpy(point_properties_rc.axis_location, "Positive Y");
            
            point_properties_rc.quadrant = 0; // A point lying on any of the coordinate axes is not a part of ANY quadrant
            printf("The Point Lies on the %s axis!\n\n", point_properties_rc.axis_location);
        }
        else if (point_rc.y == 0) // if Y is ZERO, X is NON-ZERO
        {
            if (point_rc.x < 0) // if X is -VE
                strcpy(point_properties_rc.axis_location, "Negative X");

            if (point_rc.x > 0) // if X is +VE
                strcpy(point_properties_rc.axis_location, "Positive X");
            
            point_properties_rc.quadrant = 0; // A point lying on any of the coordinate axes is not a part of ANY quadrant
            printf("The Point Lies on the %s axis!\n\n", point_properties_rc.axis_location);
        }
        else // BOTH 'X' AND 'Y' ARE NON-ZERO
        {
            point_properties_rc.axis_location[0] = '\0'; // any point lying in any of hte 4 quadrants cannot be lying on the coordinate axes..

            if (point_rc.x > 0 && point_rc.y > 0)  // X is +VE and Y is +VE
                point_properties_rc.quadrant = 1;

            else if (point_rc.x < 0 && point_rc.y > 0)  // X is -VE and Y is +VE
                point_properties_rc.quadrant = 2;

            else if (point_rc.x < 0 && point_rc.y < 0)  // X is -VE and Y is -VE
                point_properties_rc.quadrant = 3;

            else                                        // X is +VE and Y is -VE
                point_properties_rc.quadrant = 4;

            printf("The point lies in Quadrant number %d!\n\n", point_properties_rc.quadrant);
        }

    }

    return(0);
}
