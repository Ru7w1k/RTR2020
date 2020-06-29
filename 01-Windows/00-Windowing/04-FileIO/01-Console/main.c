#include <stdio.h>

int main(void)
{
    // variable declarations
    FILE *pFile = NULL;

    // code
    if (fopen_s(&pFile, "RMCLog.txt", "w") != 0)
    {
        printf("Cannot open RMCLog.txt file...\n");
        exit(0);
    }

    fprintf(pFile, "India is my country.\n");

    fclose(pFile);
    pFile = NULL;

    return(0);
}
