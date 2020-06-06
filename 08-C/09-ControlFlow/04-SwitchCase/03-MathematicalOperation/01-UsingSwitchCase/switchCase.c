#include <stdio.h> // for printf()
#include <conio.h> // for getch()

int main(void)
{
	// variable declarations
	int rmc_a, rmc_b;
	int rmc_result;

	char rmc_option, rmc_option_division;

	// code
	printf("\n\n");

	printf("Enter value for A: ");
	scanf("%d", &rmc_a);

	printf("Enter value for B: ");
	scanf("%d", &rmc_b);

	printf("Enter option in character: \n\n");
	printf("'A' or 'a' for Addition: \n");
	printf("'S' or 's' for Subtraction: \n");
	printf("'M' or 'm' for Multiplication: \n");
	printf("'D' or 'd' for Division: \n");

	printf("Enter option: ");
	rmc_option = getch();

	printf("\n\n");

	switch (rmc_option)
	{
	// fallthrough condition for 'A' and 'a'
	case 'A':
	case 'a':
		rmc_result = rmc_a + rmc_b;
		printf("Addition of A = %d and B = %d gives result %d !!\n\n", rmc_a, rmc_b, rmc_result);
		break;

	// fallthrough condition for 'S' and 's'
	case 'S':
	case 's':
		if (rmc_a >= rmc_b)
		{
			rmc_result = rmc_a - rmc_b;
			printf("Subtraction of A = %d and B = %d gives result %d !!\n\n", rmc_a, rmc_b, rmc_result);
		}
		else
		{
			rmc_result = rmc_b - rmc_a;
			printf("Subtraction of B = %d and A = %d gives result %d !!\n\n", rmc_b, rmc_a, rmc_result);
		}
		break;

	// fallthrough condition for 'M' and 'm'
	case 'M':
	case 'm':
		rmc_result = rmc_a * rmc_b;
		printf("Multiplication of A = %d and B = %d gives result %d !!\n\n", rmc_a, rmc_b, rmc_result);
		break;

	// fallthrough condition for 'D' and 'd'
	case 'D':
	case 'd':
		printf("Enter option in character: \n\n");
		printf("'Q' or 'q' or '/' for Multiplication: \n");
		printf("'R' or 'r' or '%%' for Division: \n");

		printf("Enter option: ");
		rmc_option_division = getch();

		printf("\n\n");

		switch (rmc_option_division)
		{
		case 'Q':
		case 'q':
		case '/':
			if (rmc_a > rmc_b)
			{
				rmc_result = rmc_a / rmc_b;
				printf("Division of A = %d by B = %d gives quotient = %d !!\n", rmc_a, rmc_b, rmc_result);
			}
			else
			{
				rmc_result = rmc_b / rmc_a;
				printf("Division of B = %d by A = %d gives quotient = %d !!\n", rmc_b, rmc_a, rmc_result);
			}
			break;

		case 'R':
		case 'r':
		case '%':
			if (rmc_a > rmc_b)
			{
				rmc_result = rmc_a % rmc_b;
				printf("Division of A = %d by B = %d gives remainder = %d !!\n", rmc_a, rmc_b, rmc_result);
			}
			else
			{
				rmc_result = rmc_b % rmc_a;
				printf("Division of B = %d by A = %d gives remainder = %d !!\n", rmc_b, rmc_a, rmc_result);
			}
			break;
		
		default:
			printf("Invalid character %c entered for division!! Please try again!!\n\n", rmc_option_division);
			break;
		}

		break;

	default:
		printf("Invalid character %c entered!! Please try again!!\n\n", rmc_option);
		break;
	}

	printf("Switch case block complete.\n\n");

	return (0);
}
