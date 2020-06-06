#include <stdio.h>

int main(void)
{
	// variable declarations
	int rmc_a, rmc_b, rmc_p;

	// code
	rmc_a = 8;
	rmc_b = 31;
	rmc_p = 31;

	// first if-else pair
	printf("\n\n");
	if (rmc_a < rmc_b)
	{
		printf("Entering first if-block..\n\n");
		printf("A is less that B!!\n\n");
	}
	else
	{
		printf("Entering first else-block..\n\n");
		printf("A is NOT less that B!!\n\n");
	}
	printf("First if-else pair done!!\n\n");

	// second if-else pair
	printf("\n\n");
	if (rmc_b != rmc_p)
	{
		printf("Entering second if-block..\n\n");
		printf("B is not equal to P!!\n\n");
	}
	else
	{
		printf("Entering second else-block..\n\n");
		printf("B is equal to P!!\n\n");
	}
	printf("Second if-else pair done!!\n\n");
	

	return(0);
}

