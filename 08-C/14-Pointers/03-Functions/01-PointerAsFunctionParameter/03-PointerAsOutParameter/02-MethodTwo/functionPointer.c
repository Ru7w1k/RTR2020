#include <stdio.h>

int main(void)
{
    // function declarations
    void MathematicalOperations(int, int, int *, int *, int *, int *, int *);

    // variable declarations
    int a_rc;
    int b_rc;
    int *ans_sum_rc = NULL;
    int *ans_difference_rc = NULL;
    int *ans_product_rc = NULL;
    int *ans_quotient_rc = NULL;
    int *ans_remainder_rc = NULL;

    // code
    printf("\n\n");
    printf("Enter value for A: ");
    scanf("%d", &a_rc);
    
    printf("\n\n");
    printf("Enter value for B: ");
    scanf("%d", &b_rc);

    // passing addresses to function ... function will fill them up with the values...hence the empty addresses will be given to function, and at the time of return those addresses will contain valid values
    // thus, ans_sum_rc, ans_difference_rc, ans_product_rc, ans_quotient_rc, ans_remainder_rc are called OUT PARAMETERS or PARAMETERIZED RETURN VALUES

    ans_sum_rc = (int *)malloc(sizeof(int) * 1);
    if (ans_sum_rc == NULL)
    {
        printf("Could not allocate memory to 'ans_sum'. Exiting now..\n\n");
        exit(0);
    }

    ans_difference_rc = (int *)malloc(sizeof(int) * 1);
    if (ans_difference_rc == NULL)
    {
        printf("Could not allocate memory to 'ans_difference'. Exiting now..\n\n");
        exit(0);
    }

    ans_product_rc = (int *)malloc(sizeof(int) * 1);
    if (ans_product_rc == NULL)
    {
        printf("Could not allocate memory to 'ans_product'. Exiting now..\n\n");
        exit(0);
    }

    ans_quotient_rc = (int *)malloc(sizeof(int) * 1);
    if (ans_quotient_rc == NULL)
    {
        printf("Could not allocate memory to 'ans_quotient'. Exiting now..\n\n");
        exit(0);
    }

    ans_remainder_rc = (int *)malloc(sizeof(int) * 1);
    if (ans_remainder_rc == NULL)
    {
        printf("Could not allocate memory to 'ans_remainder'. Exiting now..\n\n");
        exit(0);
    }

    MathematicalOperations(a_rc, b_rc, ans_sum_rc, ans_difference_rc, ans_product_rc, ans_quotient_rc, ans_remainder_rc);

    printf("\n\n");
    printf("--- RESULTS ---\n\n");
    printf("sum = %d\n\n", *ans_sum_rc);
    printf("difference = %d\n\n", *ans_difference_rc);
    printf("product = %d\n\n", *ans_product_rc);
    printf("quotient = %d\n\n", *ans_quotient_rc);
    printf("remainder = %d\n\n", *ans_remainder_rc);

    if (ans_sum_rc)
    {
        free(ans_sum_rc);
        ans_sum_rc = NULL;
        printf("\n\nMemory allocated for 'ans_sum' successfully freed!\n\n");
    }

    if (ans_difference_rc)
    {
        free(ans_difference_rc);
        ans_difference_rc = NULL;
        printf("\n\nMemory allocated for 'ans_difference' successfully freed!\n\n");
    }

    if (ans_product_rc)
    {
        free(ans_product_rc);
        ans_product_rc = NULL;
        printf("\n\nMemory allocated for 'ans_product' successfully freed!\n\n");
    }

    if (ans_quotient_rc)
    {
        free(ans_quotient_rc);
        ans_quotient_rc = NULL;
        printf("\n\nMemory allocated for 'ans_quotient' successfully freed!\n\n");
    }

    if (ans_remainder_rc)
    {
        free(ans_remainder_rc);
        ans_remainder_rc = NULL;
        printf("\n\nMemory allocated for 'ans_remainder' successfully freed!\n\n");
    }


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

