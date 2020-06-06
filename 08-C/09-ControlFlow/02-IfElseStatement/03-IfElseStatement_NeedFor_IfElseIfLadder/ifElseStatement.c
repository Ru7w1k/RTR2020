#include <stdio.h>

int main(void)
{
	// variable declarations
	int rmc_num;

	// code
	printf("\n\n");
	printf("Enter value for num: ");
	scanf("%d", &rmc_num);

	if (rmc_num < 0) // if - 01
	{
		printf("Num = %d is less than 0 (NEGATIVE)!!\n\n", rmc_num);
	}
	else // else - 01
	{
		if ((rmc_num > 0) && (rmc_num <= 100)) // if - 02
		{
			printf("Num = %d is between 0 and 100!!\n\n", rmc_num);
		}
		else // else - 02
		{
			if ((rmc_num > 100) && (rmc_num <= 200)) // if - 03
			{
				printf("Num = %d is between 100 and 200!!\n\n", rmc_num);
			}
			else // else - 03
			{
				if ((rmc_num > 200) && (rmc_num <= 300)) // if - 04
				{
					printf("Num = %d is between 200 and 300!!\n\n", rmc_num);
				}
				else // else - 04
				{
					if ((rmc_num > 300) && (rmc_num <= 400)) // if - 05
					{
						printf("Num = %d is between 300 and 400!!\n\n", rmc_num);
					}
					else // else - 05
					{
						if ((rmc_num > 400) && (rmc_num <= 500)) // if - 06
						{
							printf("Num = %d is between 400 and 500!!\n\n", rmc_num);
						}
						else // else - 06
						{
							printf("Num = %d is greater than 500!!\n\n", rmc_num);
						} // closing brace of else - 06

					} // closing brace of else - 05

				} // closing brace of else - 04

			} // closing brace of else - 03

		} // closing brace of else - 02

	} // closing brace of else - 01
	
	return (0);
}
