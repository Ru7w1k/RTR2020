#include <stdio.h>

#define MY_PI 3.1415926535897932

#define AMC_STRING "AstroMediComp RTR 3.0"

// if first constant is not assgined a value, it is assumed to be 0, i.e. SUNDAY will be 0
// and the rest of constant are assigned consecutive integer values from 0 onwards

// un-named enums
enum 
{
	SUNDAY,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY,
};

enum
{
	JANUARY = 1,
	FEBRUARY,
	MARCH,
	APRIL,
	MAY,
	JUNE,
	JULY,
	AUGUST,
	SEPTEMBER,
	OCTOBER,
	NOVEMBER,
	DECEMBER,
};

// named enums
enum Numbers
{
	ONE,
	TWO,
	THREE,
	FOUR,
	FIVE = 5,
	SIX,
	SEVEN,
	EIGHT,
	NINE,
	TEN,
};

enum boolean
{
	TRUE = 1,
	FALSE = 0,
};

int main(void)
{
	// local constant declaration
	const double espilon = 0.000001;

	// code
	printf("\n\n");
	printf("Local Constant Epsilon = %lf\n\n", espilon);

	printf("SUNDAY is day number = %d\n", SUNDAY);
	printf("MONDAY is day number = %d\n", MONDAY);
	printf("TUESDAY is day number = %d\n", TUESDAY);
	printf("WEDNESDAY is day number = %d\n", WEDNESDAY);
	printf("THURSDAY is day number = %d\n", THURSDAY);
	printf("FRIDAY is day number = %d\n", FRIDAY);
	printf("SATURDAY is day number = %d\n\n", SATURDAY);

	printf("One is Enum Number = %d\n", ONE);
	printf("Two is Enum Number = %d\n", TWO);
	printf("Three is Enum Number = %d\n", THREE);
	printf("Four is Enum Number = %d\n", FOUR);
	printf("Five is Enum Number = %d\n", FIVE);
	printf("Six is Enum Number = %d\n", SIX);
	printf("Seven is Enum Number = %d\n", SEVEN);
	printf("Eight is Enum Number = %d\n", EIGHT);
	printf("Nine is Enum Number = %d\n", NINE);
	printf("Ten is Enum Number = %d\n\n", TEN);

	printf("January is Month Number = %d\n", JANUARY);
	printf("February is Month Number = %d\n", FEBRUARY);
	printf("March is Month Number = %d\n", MARCH);
	printf("April is Month Number = %d\n", APRIL);
	printf("May is Month Number = %d\n", MAY);
	printf("June is Month Number = %d\n", JUNE);
	printf("July is Month Number = %d\n", JULY);
	printf("August is Month Number = %d\n", AUGUST);
	printf("September is Month Number = %d\n", SEPTEMBER);
	printf("October is Month Number = %d\n", OCTOBER);
	printf("November is Month Number = %d\n", NOVEMBER);
	printf("December is Month Number = %d\n\n", DECEMBER);

	printf("Value of TRUE is = %d\n", TRUE);
	printf("Value of FALSE is = %d\n\n", FALSE);

	printf("MY_PI macro value = %.10lf\n\n", MY_PI);
	printf("Area of circle of radius 2 units = %f\n\n", (MY_PI*2.0f*2.0f));

	printf("\n\n");

	printf(AMC_STRING);
	printf("\n\n");

	printf("AMC_STRING is: %s\n", AMC_STRING);
	printf("\n\n");
	return(0);
}

