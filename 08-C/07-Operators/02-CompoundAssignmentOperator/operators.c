#include <stdio.h>

int main(void)
{
	// variable declarations
	int a;
	int b;
	int x;

	// code
	printf("\n\n");
	printf("Enter A number: ");
	scanf("%d", &a);

	printf("\n\n");
	printf("Enter Another number: ");
	scanf("%d", &b);

	printf("\n\n");

	// these are the 5 compound assignment operatos +=, -=, *= , /=, %=
	// result of these operations is assigned to result
	x = a;
	a += b;
	printf("Addition of A = %d and B = %d Gives %d.\n", x, b, a);

	x = a;
	a -= b;
	printf("Subtraction of A = %d and B = %d Gives %d.\n", x, b, a);

	x = a;
	a *= b;
	printf("Multiplication of A = %d and B = %d Gives %d.\n", x, b, a);

	x = a;
	a /= b;
	printf("Division of A = %d and B = %d Gives Quotient %d.\n", x, b, a);

	x = a;
	a %= b;
	printf("Division of A = %d and B = %d Gives Remainder %d.\n", x, b, a);

	printf("\n\n");

	return(0);
}

