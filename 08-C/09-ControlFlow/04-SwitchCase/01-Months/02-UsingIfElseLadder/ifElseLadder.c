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

	if (rmc_month == 1) // like case 1
		printf("Month number %d is January!!\n\n", rmc_month);

	if (rmc_month == 2) // like case 2
		printf("Month number %d is February!!\n\n", rmc_month);

	if (rmc_month == 3) // like case 3
		printf("Month number %d is March!!\n\n", rmc_month);

	if (rmc_month == 4) // like case 4
		printf("Month number %d is April!!\n\n", rmc_month);

	if (rmc_month == 5) // like case 5
		printf("Month number %d is May!!\n\n", rmc_month);

	if (rmc_month == 6) // like case 6
		printf("Month number %d is June!!\n\n", rmc_month);

	if (rmc_month == 7) // like case 7
		printf("Month number %d is July!!\n\n", rmc_month);

	if (rmc_month == 8) // like case 8
		printf("Month number %d is August!!\n\n", rmc_month);

	if (rmc_month == 9) // like case 9
		printf("Month number %d is September!!\n\n", rmc_month);

	if (rmc_month == 10) // like case 10
		printf("Month number %d is October!!\n\n", rmc_month);

	if (rmc_month == 11) // like case 11
		printf("Month number %d is November!!\n\n", rmc_month);

	if (rmc_month == 12) // like case 12
		printf("Month number %d is December!!\n\n", rmc_month);

	else // like `default`
		printf("Invalid Month number %d Entered!! Please try again...\n\n", rmc_month);

	printf("if-else if-else ladder complete!!\n\n");

	return (0);
}
