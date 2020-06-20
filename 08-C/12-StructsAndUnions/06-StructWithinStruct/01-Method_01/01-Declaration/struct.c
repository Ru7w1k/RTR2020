#include <stdio.h>

struct Rectangle 
{
    struct MyPoint
    {
        int x;
        int y;
    } point01, point02;
} rect;

int main(void)
{
    // variable declarations
    int length_rc, breadth_rc, area_rc;

    // code
    printf("\n\n");
    printf("Enter Leftmost X-Coordinate of Rectangle: ");
    scanf("%d", &rect.point01.x);

    printf("\n\n");
    printf("Enter Bottommost Y-Coordinate of Rectangle: ");
    scanf("%d", &rect.point01.y);

    printf("\n\n");
    printf("Enter Rightmost X-Coordinate of Rectangle: ");
    scanf("%d", &rect.point02.x);

    printf("\n\n");
    printf("Enter Topmost Y-Coordinate of Rectangle: ");
    scanf("%d", &rect.point02.y);

    length_rc = rect.point02.y - rect.point01.y;
    if (length_rc < 0)
        length_rc *= -1;

    breadth_rc = rect.point02.x - rect.point01.x;
    if (breadth_rc < 0)
        breadth_rc *= -1;
        
    area_rc = length_rc * breadth_rc;

    printf("\n\n");
    printf("Length Of Rectangle = %d\n\n", length_rc);
    printf("Breadth Of Rectangle = %d\n\n", breadth_rc);
    printf("Area Of Rectangle = %d\n\n", area_rc);
    
    return(0);
}

