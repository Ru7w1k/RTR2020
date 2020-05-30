#include <stdio.h>

int main(void)
{
	// variable declarations
	int rmc_a;
	int rmc_b;
	int rmc_c;
	int rmc_result;

	// code
	printf("\n\n");
	printf("Enter First Integer: ");
	scanf("%d", &rmc_a);

	printf("\n\n");
	printf("Enter Second number: ");
	scanf("%d", &rmc_b);

	printf("\n\n");
	printf("Enter Third number: ");
	scanf("%d", &rmc_c);

	printf("\n\n");
	printf("If Answer = 0, It is FALSE.\n");
	printf("If Answer = 1, It is TRUE.\n\n");
	
	rmc_result = (rmc_a <= rmc_b) && (rmc_b != rmc_c);
	printf("LOGICAL AND (&&): TRUE(1) if and only if both conditions are true. FALSE(0) if any one or both are false.\n\n");
	printf("A = %d Is Less Than or Equal To B = %d AND B = %d is not equal to C = %d \t Answer = %d.\n\n", rmc_a, rmc_b, rmc_b, rmc_c, rmc_result);

	rmc_result = (rmc_b >= rmc_a) && (rmc_a == rmc_c);
	printf("LOGICAL OR (||): FALSE(0) if and only if both conditions are false. TRUE(1) if any one or both are true.\n\n");
	printf("Either B = %d Is Greater Than or Equal To A = %d OR A = %d is equal to C = %d \t Answer = %d.\n\n", rmc_b, rmc_a, rmc_a, rmc_c, rmc_result);

	rmc_result = !rmc_a;
	printf("A = %d And using LOGICAL NOT(!) operator on A gives result = %d\n\n", rmc_a, rmc_result);

	rmc_result = !rmc_b;
	printf("B = %d And using LOGICAL NOT(!) operator on B gives result = %d\n\n", rmc_b, rmc_result);

	rmc_result = !rmc_c;
	printf("C = %d And using LOGICAL NOT(!) operator on C gives result = %d\n\n", rmc_c, rmc_result);

	printf("\n\n");

	return(0);
}

