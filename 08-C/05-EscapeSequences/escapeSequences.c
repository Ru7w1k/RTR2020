#include <stdio.h>

int main(void)
{
	// code
	printf("\n\n");
	printf("Going on next line...using \\n Escape sequence\n\n");
	printf("Demonstrating \t Horizontal \t tab \t using \t \\t Escape sequence! \n\n");
	printf("\"This is a double quoted output\" done using \\\" \\\" Escape Sequence\n\n");
	printf("\'This is a single quoted output\' done using \\\' \\\' Escape Sequence\n\n");
	printf("BACKSPACE turned to BACKSPACE\b using Escape sequence \\b\n\n");

	printf("\r Demonstrating Carriage Return using \\r escape sequence\n");
	printf("Demonstrating \r Carriage Return using \\r escape sequence\n");
	printf("Demonstrating Carriage \r Return using \\r escape sequence\n");

	printf("Demonstrating \x41 using \\xhh escape sequence\n\n"); // 0x41 is hexadecimal code for 'A'
	printf("Demonstrating \102 using \\ooo escape sequence\n\n"); // 102 is octal code for 'B'

	return(0);
}

