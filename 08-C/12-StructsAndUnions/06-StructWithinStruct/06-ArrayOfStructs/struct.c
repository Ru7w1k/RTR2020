#include <stdio.h>

struct MyNumber
{
    int num;
    int num_table[10];
};

struct NumTables
{
    struct MyNumber n;
};

int main(void)
{
    // variable declarations
    struct NumTables tables_rc[10]; // array of 10 struct NumTables
    int i_rc, j_rc;

    // code
    for (i_rc = 0; i_rc < 10; i_rc++)
    {
        tables_rc[i_rc].n.num = (i_rc+1);
    }

    for (i_rc = 0; i_rc < 10; i_rc++)
    {
        printf("\n\n");
        printf("Table of %d: \n\n", tables_rc[i_rc].n.num);
        for (j_rc = 0; j_rc < 10; j_rc++)
        {
            tables_rc[i_rc].n.num_table[j_rc] = tables_rc[i_rc].n.num * (j_rc + 1);
            printf("%d * %d = %d\n", tables_rc[i_rc].n.num, (j_rc+1), tables_rc[i_rc].n.num_table[j_rc]);
        }
    }
    
    return(0);
}

