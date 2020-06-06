#include <stdio.h>

int main(void)
{
    // variable declarations
    int rmc_i, rmc_j;
    char rmc_ch_01, rmc_ch_02;

    int rmc_a, rmc_result_int;
    float rmc_f, rmc_result_float;

    int rmc_i_explicit;
    float rmc_f_explicit;

    // code
    printf("\n\n");

    // interconversion and implicit type casting between `char` and `int` types
    rmc_i = 77;
    rmc_ch_01 = rmc_i;
    printf("I = %d\n", rmc_i);
    printf("Character 1 (after ch_01 = i) = %c\n\n", rmc_ch_01);

    rmc_ch_02 = 'R';
    rmc_j = rmc_ch_02;
    printf("Character 2 = %c\n", rmc_ch_02);
    printf("J (after j = ch_02) = %d\n\n", rmc_j);

    // implicit conversion of `int` to `float`
    rmc_a = 3;
    rmc_f = 8.7f;
    rmc_result_float = rmc_a + rmc_f;
    printf("Integer a = %d and floating point number %f added gives floating point sum = %f\n\n", rmc_a, rmc_f, rmc_result_float);

    rmc_result_int = rmc_a + rmc_f;
    printf("Integer a = %d and floating point number %f added gives integer sum = %d\n\n", rmc_a, rmc_f, rmc_result_int);

    // explicit type casting using cast operator
    rmc_f_explicit = 32.123124f;
    rmc_i_explicit = (int)rmc_f_explicit;
    printf("Floating point number which will be type casted explicitly = %f\n", rmc_f_explicit);
    printf("Resultant integer after explicit type casting of %f = %d\n\n", rmc_f_explicit, rmc_i_explicit);

    return(0);
}
