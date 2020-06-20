#include <stdio.h>

struct MyData
{
    int i;
    float f;
    double d;
    char c;
};

union MyUnion
{
    int i;
    float f;
    double d;
    char c;
};

int main(void)
{
    // variable declaration
    struct MyData s_rc;
    union MyUnion u_rc;

    // code
    printf("\n\n");
    printf("Size of MyStruct = %lu\n", sizeof(s_rc));
    printf("\n\n");
    printf("Size of MyUnion = %lu\n", sizeof(u_rc));

    return(0);
}
