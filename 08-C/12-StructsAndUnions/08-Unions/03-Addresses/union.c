#include <stdio.h>

struct MyStruct
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
    struct MyStruct s_rc;
    union MyUnion u_rc;

    // code
    printf("\n\n");
    printf("Members of Struct are: \n\n");

    s_rc.i = 4;
    s_rc.f = 2.4f;
    s_rc.d = 8.465;
    s_rc.c = 'R';

    printf("s.i = %d\n\n", s_rc.i);
    printf("s.f = %f\n\n", s_rc.f);
    printf("s.d = %lf\n\n", s_rc.d);
    printf("s.c = %c\n\n", s_rc.c);

    printf("Addresses of Members of Struct are: \n\n");
    printf("s.i = %p\n\n", &s_rc.i);
    printf("s.f = %p\n\n", &s_rc.f);
    printf("s.d = %p\n\n", &s_rc.d);
    printf("s.c = %p\n\n", &s_rc.c);
    
    printf("MyStruct s = %p\n\n", &s_rc);

    printf("\n\n");
    printf("Members of Union are: \n\n");

    u_rc.i = 4;
    printf("u.i = %d\n\n", u_rc.i);

    u_rc.f = 2.4f;
    printf("u.f = %f\n\n", u_rc.f);

    u_rc.d = 8.465;
    printf("u.d = %lf\n\n", u_rc.d);

    u_rc.c = 'S';
    printf("u.c = %c\n\n", u_rc.c);

    printf("Addresses of Members of Union are: \n\n");
    printf("u.i = %p\n\n", &u_rc.i);
    printf("u.f = %p\n\n", &u_rc.f);
    printf("u.d = %p\n\n", &u_rc.d);
    printf("u.c = %p\n\n", &u_rc.c);
    
    printf("MyUnion u = %p\n\n", &u_rc);

    return(0);
}
