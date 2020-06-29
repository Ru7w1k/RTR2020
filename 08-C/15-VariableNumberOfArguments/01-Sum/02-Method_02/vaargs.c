#include <stdio.h>
#include <stdarg.h>

int main(void)
{
    // function prototypes
    int CalculateSum(int, ...);

    // variable declarations
    int ans_rc;

    // code
    printf("\n\n");

    ans_rc = CalculateSum(5, 10, 20, 30, 40, 50);
    printf("Answer = %d\n\n", ans_rc);

    ans_rc = CalculateSum(10, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2);
    printf("Answer = %d\n\n", ans_rc);

    ans_rc = CalculateSum(0);
    printf("Answer = %d\n\n", ans_rc);

    return(0);
}

int CalculateSum(int num, ...) // variadic function
{
    // function prototype
    int va_CalculateSum(int, va_list);

    // variable declaration
    int sum_rc = 0;
    va_list list_rc;

    // code
    va_start(list_rc, num);

    sum_rc = va_CalculateSum(num, list_rc);

    va_end(list_rc);
    return(sum_rc);
}

int va_CalculateSum(int num, va_list list)
{
    // variable declaration
    int n_rc = 0;
    int sum_rc = 0;

    // code
    while(num)
    {
        n_rc = va_arg(list, int);
        sum_rc += n_rc;
        num--;
    }

    return(sum_rc);
}


