#pragma once

#include "vmath.h"

vmath::mat4 Stack[100];
int top = -1;

void push(vmath::mat4 data)
{
	top++;
	Stack[top] = data;
}


vmath::mat4 pop()
{
	vmath::mat4 data = Stack[top];
	top--;
	return data;
}





