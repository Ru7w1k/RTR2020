#include <stdio.h>

int main(void)
{
	// variable declarations
	int rmc_a, rmc_b;
	int rmc_p, rmc_q;
	char rmc_ch_result_01, rmc_ch_result_02;
	int rmc_result_01, rmc_result_02;

	// code
	printf("\n\n");

	rmc_a = 7;
	rmc_b = 5;
	rmc_ch_result_01 = (rmc_a > rmc_b) ? 'A' : 'B';
	rmc_result_01 = (rmc_a > rmc_b) ? rmc_a : rmc_b;
	printf("Ternary Operator Answer 1: %c and %d.\n\n", rmc_ch_result_01, rmc_result_01);

	rmc_p = 30;
	rmc_q = 30;
	rmc_ch_result_02 = (rmc_p != rmc_q) ? 'P' : 'Q';
	rmc_result_02 = (rmc_p != rmc_q) ? rmc_p : rmc_q;
	printf("Ternary Operator Answer 2: %c and %d.\n\n", rmc_ch_result_02, rmc_result_02);

	printf("\n\n");

	return(0);
}

