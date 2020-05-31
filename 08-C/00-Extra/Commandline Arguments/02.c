#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int numbers = argc - 2;
	char operator = argv[1][0];
	int res = 0;

	if(operator == '*') res = 1;

	for(int i = 0;  i < numbers; i++)
	{
		switch(operator)
		{
			case '+':
				res += atoi(argv[2+i]);
				break;

			case '-':
				res -= atoi(argv[2+i]);
				break;
			
			case '*':
				res *= atoi(argv[2+i]);
				break;
		}
	}
	printf("\nResult of %c is = %d\n", operator, res);
	return(0); 
}

