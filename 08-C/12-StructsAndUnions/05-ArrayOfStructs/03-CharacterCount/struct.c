#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX_STRING_LENGTH 1024

struct CharacterCount
{
    char ch;
    int ch_count;
} character_and_count[] = {
    {'A', 0}, // character_and_count[0].ch = 'A' & character_and_count[0].ch_count = 0
    {'B', 0}, // character_and_count[0].ch = 'B' & character_and_count[0].ch_count = 0
    {'C', 0}, // character_and_count[0].ch = 'C' & character_and_count[0].ch_count = 0
    {'D', 0}, // character_and_count[0].ch = 'D' & character_and_count[0].ch_count = 0
    {'E', 0}, // character_and_count[0].ch = 'E' & character_and_count[0].ch_count = 0
    {'F', 0}, 
    {'G', 0}, 
    {'H', 0}, 
    {'I', 0}, 
    {'J', 0}, 
    {'K', 0}, 
    {'L', 0}, 
    {'M', 0}, 
    {'N', 0}, 
    {'O', 0}, 
    {'P', 0}, 
    {'Q', 0}, 
    {'R', 0}, 
    {'S', 0}, 
    {'T', 0}, 
    {'U', 0}, 
    {'V', 0}, 
    {'W', 0}, 
    {'X', 0}, 
    {'Y', 0}, 
    {'Z', 0}  // character_and_count[0].ch = 'Z' & character_and_count[0].ch_count = 0
};

#define SIZE_OF_ENTIRE_ARRAY_OF_STRUCTS sizeof(character_and_count)
#define SIZE_OF_ONE_STRUCT_FROM_THE_ARRAY_OF_STRUCTS sizeof(character_and_count[0])
#define NUM_ELEMENTS_IN_ARRAY (SIZE_OF_ENTIRE_ARRAY_OF_STRUCTS / SIZE_OF_ONE_STRUCT_FROM_THE_ARRAY_OF_STRUCTS)

// ENTRY POINT FUNCTION
int main(void)
{
    // variable declarations
    char str_rc[MAX_STRING_LENGTH];
    int i_rc, j_rc, actual_string_length_rc = 0;

    // code
    printf("\n\n");
    printf("Enter A String: \n\n");
    gets_s(str_rc, MAX_STRING_LENGTH);

    actual_string_length_rc = strlen(str_rc);

    printf("\n\n");
    printf("The String you have entered is: \n\n");
    printf("%s\n\n", str_rc);
    
    for (i_rc = 0; i_rc < actual_string_length_rc; i_rc++)
    {
        for (j_rc = 0; j_rc < NUM_ELEMENTS_IN_ARRAY; j_rc++)
        {
            str_rc[i_rc] = toupper(str_rc[i_rc]);

            if (str_rc[i_rc] == character_and_count[j_rc].ch)
                character_and_count[j_rc].ch_count++;
        }
    }

    printf("\n\n");
    printf("The number of occurances of all characters from the alphabet are as follows: \n\n");
    for (i_rc = 0; i_rc < NUM_ELEMENTS_IN_ARRAY; i_rc++)
    {
        printf("Character %c = %d\n", character_and_count[i_rc].ch, character_and_count[i_rc].ch_count);
    }
    printf("\n\n");

    return(0);
}

