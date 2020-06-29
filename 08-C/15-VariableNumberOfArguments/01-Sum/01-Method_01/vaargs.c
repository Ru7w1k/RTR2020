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

int CalculateSum(int num, ...)
{
    // variable declaration
    int sum_rc = 0;
    int n_rc;

    va_list list_rc;

    // code
    va_start(list_rc, num);

    while(num)
    {
        n_rc = va_arg(list_rc, int);
        sum_rc += n_rc;
        num--;
    }

    va_end(list_rc);
    return(sum_rc);
}


