#include <stdio.h>
#include <stdlib.h>

struct MyData
{
    int *ptr_i;
    int i;

    float *ptr_f;
    float f;

    double *ptr_d;
    double d;
};

int main(void)
{
    // variable declarations
    struct MyData data_rc;

    // code
    data_rc.i = 9;
    data_rc.ptr_i = &data_rc.i;

    data_rc.f = 16.546f;
    data_rc.ptr_f = &data_rc.f;

    data_rc.d = 96.48651384;
    data_rc.ptr_d = &data_rc.d;

    printf("\n\n");
    printf("i = %d\n", *(data_rc.ptr_i));
    printf("Address of i = %p\n", data_rc.ptr_i);
    
    printf("\n\n");
    printf("f = %f\n", *(data_rc.ptr_f));
    printf("Address of f = %p\n", data_rc.ptr_f);
    
    printf("\n\n");
    printf("d = %lf\n", *(data_rc.ptr_d));
    printf("Address of d = %p\n", data_rc.ptr_d);

    return(0);
}
