#include <stdio.h>

int main(void)
{
    // function prototype
    int AddInts(int, int);
    int SubtractInts(int, int);
    float AddFloats(float, float);

    // variable declarations
    typedef int (*AddIntsFnPtr)(int, int);
    AddIntsFnPtr ptrAddTwoInts_rc = NULL;
    AddIntsFnPtr ptrFunc_rc = NULL;

    typedef float (*AddFloatsFnPtr)(float, float);
    AddFloatsFnPtr ptrAddTwoFloats = NULL;

    int iAns_rc = 0;
    float fAns_rc = 0.0f;

    // code
    ptrAddTwoInts_rc = AddInts;
    iAns_rc = ptrAddTwoInts_rc(10, 20);
    printf("\n\n");
    printf("Sum of Integers = %d\n\n", iAns_rc);

    ptrFunc_rc = SubtractInts;
    iAns_rc = ptrFunc_rc(20, 5);
    printf("\n\n");
    printf("Subtract of Integers = %d\n\n", iAns_rc);

    ptrAddTwoFloats = AddFloats;
    fAns_rc = ptrAddTwoFloats(12.235f, 32.412f);
    printf("\n\n");
    printf("Sum of Floating-Point Numbers = %f\n\n", fAns_rc);

    return(0);
}

int AddInts(int a, int b)
{
    // variable declarations
    int c;

    // code
    c = a + b;
    return(c);
}

int SubtractInts(int a, int b)
{
    // variable declarations
    int c;

    // code
    if (a > b)
        c = a - b;
    else
        c = b - a;

    return(c);
}

float AddFloats(float f1, float f2)
{
    // variable declarations
    float ans;

    // code
    ans = f1 + f2;
    return(ans);
}

