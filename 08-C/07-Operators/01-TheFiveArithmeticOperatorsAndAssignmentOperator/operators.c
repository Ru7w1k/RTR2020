#include <stdio.h>

int main(void)
{
	// variable declarations
	int a;
	int b;
	int result;

	// code
	printf("\n\n");
	printf("Enter A number: ");
	scanf("%d", &a);

	printf("\n\n");
	printf("Enter Another number: ");
	scanf("%d", &b);

	printf("\n\n");

	// these are the 5 arithmetic operatos +,-,*,/,%
	// result of these operations is assigned to result
	result = a + b;
	printf("Addition of A = %d and B = %d Gives %d.\n", a, b, result);

	result = a - b;
	printf("Subtraction of A = %d and B = %d Gives %d.\n", a, b, result);

	result = a * b;
	printf("Multiplication of A = %d and B = %d Gives %d.\n", a, b, result);

	result = a / b;
	printf("Division of A = %d and B = %d Gives Quotient %d.\n", a, b, result);

	result = a % b;
	printf("Division of A = %d and B = %d Gives Remainder %d.\n", a, b, result);

	printf("\n\n");

	return(0);
}

