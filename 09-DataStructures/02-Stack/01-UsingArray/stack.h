/*
 * Stack
 */

#define STACK_SIZE 5

typedef struct _STACK {

	int top;
    int data[STACK_SIZE];

} STACK, *LPSTACK;


/* Creates a stack */
LPSTACK Create();

/* Delete a stack */
void Delete(LPSTACK);

/* Prints a stack */
void PrintStack(LPSTACK);


/*
 * Stack Operations:
 * 
 * Push():    Insert element into stack at top
 * Pop():     Remove element from stack at top
 * Peek():    Display value of element at top
 * IsEmpty(): Check if the stack is empty
 * IsFull():  Check if the stack is full
 * 
 */


/* Push int into stack pointed by LPSTACK */
void Push (LPSTACK, int);

/* Pop int from stack pointed by LPSTACK */
int  Pop  (LPSTACK);

/* Peek int from stack pointed by LPSTACK */
int  Peek (LPSTACK);

/* returns 1 if stack pointed by LPSTACK is empty, otherwise 0 */
int  IsEmpty (LPSTACK);

/* returns 1 if stack pointed by LPSTACK is full, otherwise 0 */
int  IsFull  (LPSTACK);


