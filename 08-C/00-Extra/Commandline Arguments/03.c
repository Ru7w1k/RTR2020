#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	float radius = atof(argv[1]);
	printf("Area of Circle is %f\n", 3.1415f*radius*radius);
	return(0); 
}

