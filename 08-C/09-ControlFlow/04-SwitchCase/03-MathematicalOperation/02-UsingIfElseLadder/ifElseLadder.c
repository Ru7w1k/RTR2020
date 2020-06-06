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

	if (rmc_option == 'A' || rmc_option == 'a')
	{
		rmc_result = rmc_a + rmc_b;
		printf("Addition of A = %d and B = %d gives result %d !!\n\n", rmc_a, rmc_b, rmc_result);
	}

	else if (rmc_option == 'S' || rmc_option == 's')
	{
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
	}

	else if (rmc_option == 'M' || rmc_option == 'm')
	{
		rmc_result = rmc_a * rmc_b;
		printf("Multiplication of A = %d and B = %d gives result %d !!\n\n", rmc_a, rmc_b, rmc_result);
	}

	else if (rmc_option == 'D' || rmc_option == 'd')
	{
		printf("Enter option in character: \n\n");
		printf("'Q' or 'q' or '/' for Multiplication: \n");
		printf("'R' or 'r' or '%%' for Division: \n");

		printf("Enter option: ");
		rmc_option_division = getch();

		printf("\n\n");

		if (rmc_option_division == 'Q' || rmc_option_division == 'q' || rmc_option_division == '/')
		{
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
		}

		else if (rmc_option_division == 'R' || rmc_option_division == 'r' || rmc_option_division == '%')
		{
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
		}

		else
			printf("Invalid character %c entered for division!! Please try again!!\n\n", rmc_option_division);
	}

	else
		printf("Invalid character %c entered!! Please try again!!\n\n", rmc_option);

	printf("if-else if-else ladder complete.\n\n");

	return (0);
}
