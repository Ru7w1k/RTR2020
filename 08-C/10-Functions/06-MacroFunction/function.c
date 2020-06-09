#include <stdio.h> 

#define MAX_NUMBER(a, b) ((a > b) ? a : b)

int main(int argc, char *argv[], char *envp[])
{
    // variable declarations
    int rmc_iNum_01;
    int rmc_iNum_02;
    int rmc_iResult;

    float rmc_fNum_01;
    float rmc_fNum_02;
    float rmc_fResult;

    // code
    // COMPARING INTEGER VALUES
    printf("\n\n");
    printf("Entry an Integer Number: \n\n");
    scanf("%d", &rmc_iNum_01);

    printf("\n\n");
    printf("Entry another Integer Number: \n\n");
    scanf("%d", &rmc_iNum_02);

    rmc_iResult = MAX_NUMBER(rmc_iNum_01, rmc_iNum_02);
    printf("\n\n");
    printf("Result of Macro function MAX_NUMBER() = %d\n", rmc_iResult);

    // COMPARING FLOAT VALUES
    printf("\n\n");
    printf("Entry an Float Number: \n\n");
    scanf("%f", &rmc_fNum_01);

    printf("\n\n");
    printf("Entry another Float Number: \n\n");
    scanf("%f", &rmc_fNum_02);

    rmc_fResult = MAX_NUMBER(rmc_fNum_01, rmc_fNum_02);
    printf("\n\n");
    printf("Result of Macro function MAX_NUMBER() = %f\n", rmc_fResult);

    printf("\n\n");
    return(0);
}
