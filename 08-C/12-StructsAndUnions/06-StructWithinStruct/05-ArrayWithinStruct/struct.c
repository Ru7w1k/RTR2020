#include <stdio.h>

struct MyNumber
{
    int num;
    int num_table[10];
};

struct NumTables
{
    struct MyNumber a;
    struct MyNumber b;
    struct MyNumber c;
};

int main(void)
{
    // variable declarations
    struct NumTables tables_rc;
    int i_rc;

    // code
    tables_rc.a.num = 2;
    for (i_rc = 0; i_rc < 10; i_rc++)
        tables_rc.a.num_table[i_rc] = tables_rc.a.num * (i_rc + 1);
    printf("\n\n");
    printf("Table of %d: \n\n", tables_rc.a.num);
    for (i_rc = 0; i_rc < 10; i_rc++)
        printf("%d * %d = %d\n", tables_rc.a.num, (i_rc+1), tables_rc.a.num_table[i_rc]);
    
    tables_rc.b.num = 3;
    for (i_rc = 0; i_rc < 10; i_rc++)
        tables_rc.b.num_table[i_rc] = tables_rc.b.num * (i_rc + 1);
    printf("\n\n");
    printf("Table of %d: \n\n", tables_rc.b.num);
    for (i_rc = 0; i_rc < 10; i_rc++)
        printf("%d * %d = %d\n", tables_rc.b.num, (i_rc+1), tables_rc.b.num_table[i_rc]);

    tables_rc.c.num = 4;
    for (i_rc = 0; i_rc < 10; i_rc++)
        tables_rc.c.num_table[i_rc] = tables_rc.c.num * (i_rc + 1);
    printf("\n\n");
    printf("Table of %d: \n\n", tables_rc.c.num);
    for (i_rc = 0; i_rc < 10; i_rc++)
        printf("%d * %d = %d\n", tables_rc.c.num, (i_rc+1), tables_rc.c.num_table[i_rc]);

    return(0);
}

