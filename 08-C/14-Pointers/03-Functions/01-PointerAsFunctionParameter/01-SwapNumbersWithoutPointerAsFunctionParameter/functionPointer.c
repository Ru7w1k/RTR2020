#include <stdio.h>

int main(void)
{
    // function declarations
    void SwapNumbers(int, int);

    // variable declarations
    int a_rc;
    int b_rc;

    // code
    printf("\n\n");
    printf("Enter value for A: ");
    scanf("%d", &a_rc);
    
    printf("\n\n");
    printf("Enter value for B: ");
    scanf("%d", &b_rc);

    printf("\n\n");
    printf("--- Before Swapping ---\n\n");
    printf("Value of 'A' = %d\n\n", a_rc);
    printf("Value of 'B' = %d\n\n", b_rc);

    SwapNumbers(a_rc, b_rc);

    printf("\n\n");
    printf("--- After Swapping ---\n\n");
    printf("Value of 'A' = %d\n\n", a_rc);
    printf("Value of 'B' = %d\n\n", b_rc);

    return(0);
}

// value of a is copied into x and value of b is copied into y
// swapping takes place between x and y and not between a and b
void SwapNumbers(int x, int y)
{
    // variable declarations
    int temp;

    // code
    printf("\n\n");
    printf("--- Before Swapping ---\n\n");
    printf("Value of 'X' = %d\n\n", x);
    printf("Value of 'Y' = %d\n\n", y);

    temp = x;
    x = y;
    y = temp;

    printf("\n\n");
    printf("--- After Swapping ---\n\n");
    printf("Value of 'X' = %d\n\n", x);
    printf("Value of 'Y' = %d\n\n", y);

}

