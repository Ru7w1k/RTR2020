#include <stdio.h>

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
    union MyUnion u1_rc, u2_rc;

    // code
    // MyUnion u1
    printf("\n\n");
    printf("Members of Union u1 are: \n\n");

    u1_rc.i = 4;
    u1_rc.f = 2.4f;
    u1_rc.d = 8.465;
    u1_rc.c = 'S';

    printf("u1.i = %d\n\n", u1_rc.i);
    printf("u1.f = %f\n\n", u1_rc.f);
    printf("u1.d = %lf\n\n", u1_rc.d);
    printf("u1.c = %c\n\n", u1_rc.c);

    printf("Addresses of Members of Union u1 are: \n\n");
    printf("u1.i = %p\n\n", &u1_rc.i);
    printf("u1.f = %p\n\n", &u1_rc.f);
    printf("u1.d = %p\n\n", &u1_rc.d);
    printf("u1.c = %p\n\n", &u1_rc.c);
    
    printf("MyUnion u1 = %p\n\n", &u1_rc);

    // MyUnion u2
    printf("\n\n");
    printf("Members of Union u2 are: \n\n");

    u2_rc.i = 4;
    printf("u2.i = %d\n\n", u2_rc.i);

    u2_rc.f = 2.4f;
    printf("u2.f = %f\n\n", u2_rc.f);

    u2_rc.d = 8.465;
    printf("u2.d = %lf\n\n", u2_rc.d);

    u2_rc.c = 'S';
    printf("u2.c = %c\n\n", u2_rc.c);

    printf("Addresses of Members of Union u2 are: \n\n");
    printf("u2.i = %p\n\n", &u2_rc.i);
    printf("u2.f = %p\n\n", &u2_rc.f);
    printf("u2.d = %p\n\n", &u2_rc.d);
    printf("u2.c = %p\n\n", &u2_rc.c);
    
    printf("MyUnion u2 = %p\n\n", &u2_rc);

    return(0);
}
