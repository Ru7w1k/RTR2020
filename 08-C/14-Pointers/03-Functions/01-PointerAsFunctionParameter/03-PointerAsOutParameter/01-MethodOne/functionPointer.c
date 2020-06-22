#include <stdio.h>

int main(void)
{
    // function declarations
    void MathematicalOperations(int, int, int *, int *, int *, int *, int *);

    // variable declarations
    int a_rc;
    int b_rc;
    int ans_sum_rc;
    int ans_difference_rc;
    int ans_product_rc;
    int ans_quotient_rc;
    int ans_remainder_rc;

    // code
    printf("\n\n");
    printf("Enter value for A: ");
    scanf("%d", &a_rc);
    
    printf("\n\n");
    printf("Enter value for B: ");
    scanf("%d", &b_rc);

    // passing addresses to function ... function will fill them up with the values...hence the empty addresses will be given to function, and at the time of return those addresses will contain valid values
    // thus, ans_sum_rc, ans_difference_rc, ans_product_rc, ans_quotient_rc, ans_remainder_rc are called OUT PARAMETERS or PARAMETERIZED RETURN VALUES

    MathematicalOperations(a_rc, b_rc, &ans_sum_rc, &ans_difference_rc, &ans_product_rc, &ans_quotient_rc, &ans_remainder_rc);

    printf("\n\n");
    printf("--- RESULTS ---\n\n");
    printf("sum = %d\n\n", ans_sum_rc);
    printf("difference = %d\n\n", ans_difference_rc);
    printf("product = %d\n\n", ans_product_rc);
    printf("quotient = %d\n\n", ans_quotient_rc);
    printf("remainder = %d\n\n", ans_remainder_rc);

    return(0);
}

void MathematicalOperations(int x, int y, int *sum, int *difference, int *product, int *quotient, int *remainder)
{
    // code
    *sum = x + y;           // value at address 'sum' = x + y
    *difference = x - y;    // value at address 'difference' = x - y
    *product = x * y;       // value at address 'product' = x * y
    *quotient = x / y;      // value at address 'quotient' = x / y
    *remainder = x % y;     // value at address 'remainder' = x % y
}

