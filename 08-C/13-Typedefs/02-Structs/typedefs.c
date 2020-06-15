#include <stdio.h>

#define MAX_NAME_LENGTH 100

struct Employee
{
    char name[MAX_NAME_LENGTH];
    unsigned int age;
    char gender;
    double salary;
};

struct MyData
{
    int i;
    float f;
    double d;
    char c;
};


int main(void)
{
    // typedefs
    typedef struct Employee MY_EMPLOYEE_TYPE;
    typedef struct MyData MY_DATA_TYPE;

    // variable declarations
    struct Employee emp_rc = {"Funny", 23, 'M', 10000.00};
    MY_EMPLOYEE_TYPE emp_typedef_rc = {"Bunny", 25, 'F', 25000.0};

    struct MyData md_rc = {30, 11.24f, 12.421342, 'X'};
    MY_DATA_TYPE md_typedef_rc;

    md_typedef_rc.i = 9;
    md_typedef_rc.f = 1.6f;
    md_typedef_rc.d = 8.36419;
    md_typedef_rc.c = 'R';

    printf("\n\n");
    printf("Struct Employee: \n\n");
    printf("emp.name   = %s\n", emp_rc.name);
    printf("emp.age    = %d\n", emp_rc.age);
    printf("emp.gender = %c\n", emp_rc.gender);
    printf("emp.salary = %lf\n", emp_rc.salary);

    printf("\n\n");
    printf("MY_EMPLOYEE_TYPE: \n\n");
    printf("emp_typedef.name   = %s\n", emp_typedef_rc.name);
    printf("emp_typedef.age    = %d\n", emp_typedef_rc.age);
    printf("emp_typedef.gender = %c\n", emp_typedef_rc.gender);
    printf("emp_typedef.salary = %lf\n", emp_typedef_rc.salary);
    
    printf("\n\n");
    printf("Struct MyData: \n\n");
    printf("md.i = %d\n",  md_rc.i);
    printf("md.f = %f\n",  md_rc.f);
    printf("md.d = %lf\n", md_rc.d);
    printf("md.c = %c\n",  md_rc.c);

    printf("\n\n");
    printf("MY_DATA_TYPE: \n\n");
    printf("md_typedef.i = %d\n",  md_typedef_rc.i);
    printf("md_typedef.f = %f\n",  md_typedef_rc.f);
    printf("md_typedef.d = %lf\n", md_typedef_rc.d);
    printf("md_typedef.c = %c\n",  md_typedef_rc.c);

    printf("\n\n");

    return(0);
}
