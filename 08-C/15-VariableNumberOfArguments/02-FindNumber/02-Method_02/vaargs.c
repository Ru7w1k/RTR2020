#include <stdio.h>
#include <stdarg.h>

#define NUM_TO_BE_FOUND 3
#define NUM_ELEMENTS 10

int main(void)
{
    // function prototypes
    void FindNumber(int, int, ...);

    // variable declarations
    int ans_rc;

    // code
    printf("\n\n");

    FindNumber(NUM_TO_BE_FOUND, NUM_ELEMENTS, 3, 5, 4, 1, 3, 4, 6, 9, 3, 5);

    return(0);
}

void FindNumber(int num_te_be_found, int num, ...) // variadic function
{
    // function declaration
    int va_FindNumber(int, int, va_list);

    // variable declaration
    int count_rc = 0;
    va_list numbers_list_rc;

    // code
    va_start(numbers_list_rc, num);

    count_rc = va_FindNumber(num_te_be_found, num, numbers_list_rc);

    if (count_rc == 0)
        printf("Number %d could not be found!!\n\n", num_te_be_found);
    else
        printf("Number %d found %d times!!\n\n", num_te_be_found, count_rc);

    va_end(numbers_list_rc);
}

int va_FindNumber(int num_te_be_found, int num, va_list list)
{
    // variable declaration
    int count_rc = 0;
    int n_rc;

    // code
    while (num)
    {
        n_rc = va_arg(list, int);
        if (n_rc == num_te_be_found)
            count_rc++;
        num--;
    }

    return(count_rc);
}

