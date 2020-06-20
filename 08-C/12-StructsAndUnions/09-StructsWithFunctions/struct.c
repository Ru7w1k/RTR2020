#include <stdio.h>

// defining struct
struct MyData
{
    int i;
    float f;
    double d;
    char c;
};

int main(void)
{
    // function declaration
    struct MyData AddStructMembers(struct MyData, struct MyData, struct MyData);

    // variable declaration
    struct MyData d1_rc, d2_rc, d3_rc, ans_rc;

    // code

    // data1
    printf("\n\n\n\n");
    printf("****** DATA 1 ****** \n\n");
    printf("Enter Integer Value for 'i' of 'struct MyData d1': ");
    scanf("%d", &d1_rc.i);

    printf("\n\n");
    printf("Enter Floating-Point Value for 'f' of 'struct MyData d1': ");
    scanf("%f", &d1_rc.f);

    printf("\n\n");
    printf("Enter Double Value for 'd' of 'struct MyData d1': ");
    scanf("%lf", &d1_rc.d);

    printf("\n\n");
    printf("Enter Character Value for 'c' of 'struct MyData d1': ");
    d1_rc.c = getch();
    printf("%c", d1_rc.c);

    // data2
    printf("\n\n\n\n");
    printf("****** DATA 2 ****** \n\n");
    printf("Enter Integer Value for 'i' of 'struct MyData d1': ");
    scanf("%d", &d2_rc.i);

    printf("\n\n");
    printf("Enter Floating-Point Value for 'f' of 'struct MyData d1': ");
    scanf("%f", &d2_rc.f);

    printf("\n\n");
    printf("Enter Double Value for 'd' of 'struct MyData d1': ");
    scanf("%lf", &d2_rc.d);

    printf("\n\n");
    printf("Enter Character Value for 'c' of 'struct MyData d1': ");
    d2_rc.c = getch();
    printf("%c", d2_rc.c);

    // data3
    printf("\n\n\n\n");
    printf("****** DATA 3 ****** \n\n");
    printf("Enter Integer Value for 'i' of 'struct MyData d1': ");
    scanf("%d", &d3_rc.i);

    printf("\n\n");
    printf("Enter Floating-Point Value for 'f' of 'struct MyData d1': ");
    scanf("%f", &d3_rc.f);

    printf("\n\n");
    printf("Enter Double Value for 'd' of 'struct MyData d1': ");
    scanf("%lf", &d3_rc.d);

    printf("\n\n");
    printf("Enter Character Value for 'c' of 'struct MyData d1': ");
    d3_rc.c = getch();
    printf("%c", d3_rc.c);

    // CALLING FUNCTION AddStructMembers() WHICH ACCEPTS THREE VARIABLES OF TYPE 
    // struct MyData AS PARAMETERS AND ADDS UP THE RESPECTIVE MEMBERS AND RETURNS 
    // THE ANSWER IN ANOTHER struct OF SAME TYPE
    ans_rc = AddStructMembers(d1_rc, d2_rc, d3_rc);

    printf("\n\n\n\n");
    printf("******* ANSWER ****** \n\n");
    printf("answer_data.i = %d\n", ans_rc.i);
    printf("answer_data.f = %f\n", ans_rc.f);
    printf("answer_data.d = %lf\n", ans_rc.d);

    ans_rc.c = d1_rc.c;
    printf("answer_data.c (from d1) = %c\n\n", ans_rc.c);

    ans_rc.c = d2_rc.c;
    printf("answer_data.c (from d2) = %c\n\n", ans_rc.c);

    ans_rc.c = d3_rc.c;
    printf("answer_data.c (from d3) = %c\n\n", ans_rc.c);

    return(0);
}

struct MyData AddStructMembers(struct MyData md1, struct MyData md2, struct MyData md3)
{
    // variable declarations
    struct MyData answer;

    // code
    answer.i = md1.i + md2.i + md3.i;
    answer.f = md1.f + md2.f + md3.f;
    answer.d = md1.d + md2.d + md3.d;

    return (answer);
}


