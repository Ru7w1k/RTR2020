#include <cstring>


float angle = 0.0f;
char axiom[] = "F";
char sentence[100000] = { 0 };
float len = 100.0f;

// rule1: F -> FF+[+F-F-F]-[-F+F+F]
char ruleText[] = "FF+[+F-F-F]-[-F+F+F]";

void generate()
{
	len *= 0.5f;
	sentence[0] = 'F';
	char nextSentence[100000] = { 0 };
	for (int i = 0, j = 0; i < strlen(sentence); i++)
	{

		if (sentence[i] == 'F')
		{
			for (int k = 0; k < strlen(ruleText); k++)
			{
				nextSentence[j++] = ruleText[k];
			}
		}
		else
		{
			nextSentence[j++] = sentence[i];
		}
	}
	strcpy_s(sentence, sizeof(sentence), nextSentence);
}





