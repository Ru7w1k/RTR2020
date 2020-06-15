#include <stdio.h>

// global typedef
typedef int MY_INT; // type int has been redefined as MY_INT, now MY_INT can be treated as int

int main(void)
{
    // function prototype
    MY_INT Add(MY_INT, MY_INT);

    // typedefs
    typedef int MY_INT; // type int has been redefined as MY_INT, now MY_INT can be treated as int
    typedef float RMC_FLOAT; // type float has been redefined as RMC_FLOAT, now RMC_FLOAT can be treated as float
    typedef char CHARACTER; // type char has been redefined as CHARCTER, now CHARCTER can be treated as char
    typedef double MY_DOUBLE; // type double has been redefined as MY_DOUBLE, now MY_DOUBLE can be treated as double

    // just like in Win32SDK!
    typedef unsigned int UINT;
    typedef UINT HANDLE;
    typedef HANDLE HWND;
    typedef HANDLE HINSTANCE;


    // variable declarations
    MY_INT rmc_a = 10, rmc_i;
    MY_INT iArray_rmc[] = {8, 16, 24, 32, 40, 48, 56, 64, 72, 80};

    RMC_FLOAT f_rmc = 21.9f;
    const RMC_FLOAT f_rmc_pi = 3.14f;

    CHARACTER rmc_ch = '*';
    CHARACTER rmc_chArr01[] = "Hello";
    CHARACTER rmc_chArr02[][10] = {"RTR", "Batch", "2020-2021"};

    MY_DOUBLE rmc_d = 8.246345;

    // just random numbers: they have nothing to do with any windows's handle or instance;
    UINT rmc_uint = 4654;
    HANDLE rmc_handle = 643;
    HWND rmc_hwnd = 6543;
    HINSTANCE rmc_hinstance = 8435;

    // code
    printf("\n\n");
    printf("Type MY_INT variable a = %d", rmc_a);

    printf("\n\n");

    for(rmc_i = 0; rmc_i < (sizeof(iArray_rmc)/sizeof(int)); rmc_i++)
    {
        printf("Type MY_INT array variable iArray[%d] = %d\n", rmc_i, iArray_rmc[rmc_i]);
    }

    printf("\n\n");

    printf("Type RMC_FLOAT variable f = %f\n", f_rmc);
    printf("Type RMC_FLOAT constant f_rmc_pi = %f\n", f_rmc_pi);

    printf("\n\n");
    printf("Type MY_DOUBLE variable d = %lf\n", rmc_d);

    printf("\n\n");
    printf("Type CHARACTER variable ch = %c\n", rmc_ch);

    printf("\n\n");
    printf("Type CHARACTER array variable chArray_01 = %s\n", rmc_chArr01);

    printf("\n\n");
    for (rmc_i = 0; rmc_i < (sizeof(rmc_chArr02) / sizeof(rmc_chArr02[0])); rmc_i++)
    {
        printf("%s\t", rmc_chArr02[rmc_i]);
    }
    printf("\n\n");

    printf("\n\n");
    printf("Type UINT variable uint = %u\n\n", rmc_uint);
    printf("Type HANDLE variable handle = %u\n\n", rmc_handle);
    printf("Type HWND variable hwnd = %u\n\n", rmc_hwnd);
    printf("Type HINSTANCE variable hInstance = %u\n\n", rmc_hinstance);
    printf("\n\n");

    MY_INT rmc_x = 80;
    MY_INT rmc_y = 40;
    MY_INT rmc_ret;

    rmc_ret = Add(rmc_x, rmc_y);
    printf("ret = %d", rmc_ret);

    return(0);
}

MY_INT Add(MY_INT a, MY_INT b)
{
    // code
    MY_INT c;
    c = a + b;
    return(c);
}



