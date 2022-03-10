
#import "vmath.h"
using namespace vmath;

#define SIZE 10

mat4 stack[SIZE];
int sp = -1;

void push(mat4 data);
mat4 pop();
mat4 peek();
void resetStack();

void resetStack()
{
    sp = -1;
}

void push(mat4 data)
{
    if (sp == SIZE- 1)
    {
        printf("STACK OVERFLOW\n");
    }
    else
    {
        sp++;
        stack[sp] = data;
    }
}

mat4 pop()
{
    mat4 data;
    if (sp > -1)
    {
        data = stack[sp];
        sp--;
    }
    else
    {
        printf("STACK UNDERFLOW\n");
    }
    return data;
}

mat4 peek()
{
    if (sp > -1)
    {
        return stack[sp];
    }
    return (mat4)0;
}
