#include <stdio.h>

int main(void)
{
	// variable declarations
	int rmc_month;

	// code
	printf("\n\n");
	printf("Enter number of month (1 to 12): ");
	scanf("%d", &rmc_month);

	printf("\n\n");

	switch (rmc_month)
	{
	case 1: // like `if`
		printf("Month number %d is January!!\n\n", rmc_month);
		break;

	case 2: // like `else if`
		printf("Month number %d is February!!\n\n", rmc_month);
		break;

	case 3: // like `else if`
		printf("Month number %d is March!!\n\n", rmc_month);
		break;

	case 4: // like `else if`
		printf("Month number %d is April!!\n\n", rmc_month);
		break;

	case 5: // like `else if`
		printf("Month number %d is May!!\n\n", rmc_month);
		break;

	case 6: // like `else if`
		printf("Month number %d is June!!\n\n", rmc_month);
		break;

	case 7: // like `else if`
		printf("Month number %d is July!!\n\n", rmc_month);
		break;

	case 8: // like `else if`
		printf("Month number %d is August!!\n\n", rmc_month);
		break;

	case 9: // like `else if`
		printf("Month number %d is September!!\n\n", rmc_month);
		break;

	case 10: // like `else if`
		printf("Month number %d is October!!\n\n", rmc_month);
		break;

	case 11: // like `else if`
		printf("Month number %d is November!!\n\n", rmc_month);
		break;

	case 12: // like `else if`
		printf("Month number %d is December!!\n\n", rmc_month);
		break;

	default: // like ending optinal `else`
		printf("Invalid Month number %d Entered!! Please try again...\n\n", rmc_month);
		break;
	}

	printf("Switch case block complete!!\n\n");

	return (0);
}
