#include <stdio.h> // for printf()
#include <conio.h> // for getch()

// ASCII values for 'A' to 'Z' => 65 to 90
#define CHAR_ALPHABER_UPPER_CASE_BEGIN 65
#define CHAR_ALPHABER_UPPER_CASE_END 90

// ASCII values for 'a' to 'z' => 97 to 122
#define CHAR_ALPHABER_LOWER_CASE_BEGIN 97
#define CHAR_ALPHABER_LOWER_CASE_END 122

// ASCII value for '0' to '9' => 48 to 57
#define CHAR_DIGIT_BEGIN 48
#define CHAR_DIGIT_END 57

int main(void)
{
	// variable declarations
	char rmc_ch;
	int rmc_ch_value;

	// code
	printf("\n\n");

	printf("Enter Character: ");
	rmc_ch = getch();

	printf("\n\n");

	if ((rmc_ch == 'A' || rmc_ch == 'a') || (rmc_ch == 'E' || rmc_ch == 'e') || (rmc_ch == 'I' || rmc_ch == 'i') || (rmc_ch == 'O' || rmc_ch == 'o') || (rmc_ch == 'U' || rmc_ch == 'u'))
	{
		printf("Character \'%c\' is a VOWEL CHARACTER!!\n\n", rmc_ch);
	}
	else
	{
		rmc_ch_value = (int)rmc_ch;
		// if character ASCII value is between 65 AND 90 OR 94 AND 122,
		// it is still a valid letter of the Alphabet, but it is a CONSONANT
		if ((rmc_ch_value >= CHAR_ALPHABER_UPPER_CASE_BEGIN && rmc_ch_value <= CHAR_ALPHABER_UPPER_CASE_END) || (rmc_ch_value >= CHAR_ALPHABER_LOWER_CASE_BEGIN && rmc_ch_value <= CHAR_ALPHABER_LOWER_CASE_END))
		{
			printf("Character \'%c\' is a CONSONANT CHARACTER!!\n\n", rmc_ch);
		}
		else if (rmc_ch_value >= CHAR_DIGIT_BEGIN && rmc_ch_value <= CHAR_DIGIT_END)
		{
			printf("Character \'%c\' is a DIGIT CHARACTER!!\n\n", rmc_ch);
		}
		else
		{
			printf("Character \'%c\' is a SPECIAL CHARACTER!!\n\n", rmc_ch);
		}
	}

	return (0);
}
