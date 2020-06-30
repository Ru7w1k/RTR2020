#include <stdio.h>
#include <stdlib.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
    // function prototype
    char *ReplaceVowelsWithHashSymbol(char *);

    // variable declarations
    char string_rc[MAX_STRING_LENGTH];
    char *replaced_string_rc = NULL;

    // code
    printf("\n\n");
    printf("Enter String: ");
    gets_s(string_rc, MAX_STRING_LENGTH);

    replaced_string_rc = ReplaceVowelsWithHashSymbol(string_rc);
    if (replaced_string_rc == NULL)
    {
        printf("ReplaceVowelsWithHashSymbol() failed! exiting now..\n");
        exit(0);
    }

    printf("\n\n");
    printf("Replaced string is: \n\n");
    printf("%s\n", replaced_string_rc);

    if (replaced_string_rc)
    {
        free(replaced_string_rc);
        replaced_string_rc = NULL;
    }

    return(0);
}

char *ReplaceVowelsWithHashSymbol(char *s)
{
    // function prototype
    void MyStrcpy(char *, char *);
    int  MyStrlen(char *);

    // variable declarations
    char *new_string_rc = NULL;
    char i_rc;

    // code
    new_string_rc = (char *)malloc(MyStrlen(s) * sizeof(char));
    if (new_string_rc == NULL)
    {
        printf("COULD NOT ALLOCATE MEMORY FOR NEW STRING!\n\n");
        return(NULL);
    }

    MyStrcpy(new_string_rc, s);

    for (i_rc = 0; i_rc < MyStrlen(new_string_rc); i_rc++)
    {
        switch (new_string_rc[i_rc])
        {
        case 'A':
        case 'a':
        case 'E':
        case 'e':
        case 'I':
        case 'i':
        case 'O':
        case 'o':
        case 'U':
        case 'u':
            new_string_rc[i_rc] = '#';
            break;
        
        default:
            break;
        }
    }
    return(new_string_rc);
}

void MyStrcpy(char *str_destination, char *str_source)
{
    // function prototype
    int MyStrlen(char *);

    // variable declarations
    int iStringLength_rmc = 0;
    int rmc_j;

    // code
    iStringLength_rmc = MyStrlen(str_source);
    for (rmc_j = 0; rmc_j < iStringLength_rmc; rmc_j++)
        *(str_destination + rmc_j) = *(str_source + rmc_j);

    *(str_destination + rmc_j) = '\0';
}

int MyStrlen(char *str)
{
    // variable declarations
    int rmc_j;
    int rmc_string_length = 0;

    // code
    // detecting exact length of the string, by detecting the first occurence of null-terminating character (\0)
    for (rmc_j = 0; rmc_j < MAX_STRING_LENGTH; rmc_j++)
    {
        if (*(str + rmc_j) == '\0')
            break;
        else
            rmc_string_length++;
    }
    
    return(rmc_string_length);
}


