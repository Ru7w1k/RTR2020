#include <stdio.h>

int main(void)
{
	// variable declarations
	int rmc_a;
	int rmc_b;
	int rmc_result;

	// code
	printf("\n\n");
	printf("Enter One Integer: ");
	scanf("%d", &rmc_a);

	printf("\n\n");
	printf("Enter Another number: ");
	scanf("%d", &rmc_b);

	printf("\n\n");
	printf("If Answer = 0, It is FALSE.\n");
	printf("If Answer = 1, It is TRUE.\n\n");
	
	rmc_result = (rmc_a < rmc_b);
	printf("(a < b) A = %d Is Less Than B = %d \t Answer = %d.\n", rmc_a, rmc_b, rmc_result);

	rmc_result = (rmc_a > rmc_b);
	printf("(a > b) A = %d Is Greater Than B = %d \t Answer = %d.\n", rmc_a, rmc_b, rmc_result);

	rmc_result = (rmc_a <= rmc_b);
	printf("(a <= b) A = %d Is Less Than or Equal To B = %d \t Answer = %d.\n", rmc_a, rmc_b, rmc_result);

	rmc_result = (rmc_a >= rmc_b);
	printf("(a >= b) A = %d Is Less Than or Equal To B = %d \t Answer = %d.\n", rmc_a, rmc_b, rmc_result);

	rmc_result = (rmc_a == rmc_b);
	printf("(a == b) A = %d Is Equal To B = %d \t Answer = %d.\n", rmc_a, rmc_b, rmc_result);

	rmc_result = (rmc_a != rmc_b);
	printf("(a != b) A = %d Is Not Equal To B = %d \t Answer = %d.\n", rmc_a, rmc_b, rmc_result);

	printf("\n\n");

	return(0);
}

