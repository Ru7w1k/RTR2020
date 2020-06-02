#include <stdio.h>

int main(void)
{
	// function prototypes
	void PrintBinaryFormOfNumber(unsigned int);

	// variable declarations
	unsigned int rmc_a;
	unsigned int rmc_result;

	// code
	printf("\n\n");
	printf("Enter First Integer: ");
	scanf("%u", &rmc_a);

	printf("\n\n\n\n");
	rmc_result = ~rmc_a;
	printf("Bitwise COMPLEMENTing of \nA = %d (decimal) gives result %d (decimal).\n\n", rmc_a, rmc_result);

	PrintBinaryFormOfNumber(rmc_a);
	PrintBinaryFormOfNumber(rmc_result);

	return(0);
}

void PrintBinaryFormOfNumber(unsigned int decimal_number)
{
	// variable declaration
	unsigned int rmc_q, rmc_r;
	unsigned int rmc_num;
	unsigned int rmc_binary_array[8];
	int rmc_i;

	// code
	for (rmc_i = 0; rmc_i < 8; rmc_i++)
	{
		rmc_binary_array[rmc_i] = 0;
	}

	printf("The binary form of decimal integer %d is \t=\t", decimal_number);
	rmc_num = decimal_number;
	rmc_i = 7;

	while (rmc_num != 0)
	{
		rmc_q = rmc_num / 2;
		rmc_r = rmc_num % 2;
		rmc_binary_array[rmc_i] = rmc_r;
		rmc_num = rmc_q;
		rmc_i--;
	}

	for (rmc_i = 0; rmc_i < 8; rmc_i++)
	{
		printf("%u", rmc_binary_array[rmc_i]);
	}

	printf("\n\n");
}

