#include <stdio.h>

int main(void)
{
	// variable declarations
	int rmc_age;

	// code
	printf("\n\n");
	printf("Enter Age: ");
	scanf("%d", &rmc_age);

	if (rmc_age >= 18)
	{
		printf("Entering if-block..\n\n");
		printf("You are eligible for voting!!\n\n");
	}
	else
	{
		printf("Entering else-block..\n\n");
		printf("You are NOT eligible for voting!!\n\n");
	}

	printf("Bye!!\n\n");
	return(0);
}

