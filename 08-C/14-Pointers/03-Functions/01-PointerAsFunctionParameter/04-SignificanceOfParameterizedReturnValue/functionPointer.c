#include <stdio.h>

enum
{
    NEGATIVE = -1,
    ZERO,
    POSITIVE
};

int main(void)
{
    // function declarations
    int Difference(int, int, int *);

    // variable declarations
    int a_rc;
    int b_rc;
    int ans_rc, ret_rc;

    // code
    printf("\n\n");
    printf("Enter value for A: ");
    scanf("%d", &a_rc);
    
    printf("\n\n");
    printf("Enter value for B: ");
    scanf("%d", &b_rc);

    ret_rc = Difference(a_rc, b_rc, &ans_rc);

    printf("\n\n");
    printf("Difference of %d and %d = %d\n\n", a_rc, b_rc, ans_rc);

    if (ret_rc == POSITIVE)
        printf("The difference between %d and %d is Positive!\n\n", a_rc, b_rc);

    else if (ret_rc == NEGATIVE)
        printf("The difference between %d and %d is Negative!\n\n", a_rc, b_rc);
    
    else
        printf("The difference between %d and %d is Zero!\n\n", a_rc, b_rc);

    return(0);
}

// this function performs two tasks, it calculates the difference between x and y and return in via diff
// and the return value of function specified whether the result was NEGATIVE, POSITIVE or ZERO
int Difference(int x, int y, int *diff)
{
    // code
    *diff = x - y;

    if (*diff > 0)
        return(POSITIVE);

    else if (*diff < 0)
        return(NEGATIVE);

    else
        return(ZERO);

}

