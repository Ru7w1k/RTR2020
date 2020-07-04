/*
 * Queue
 */

#define QUEUE_SIZE 10

typedef struct _QUEUE {

	int front;
    int rear;
    int data[QUEUE_SIZE];

} QUEUE, *LPQUEUE;


/* Creates a queue */
LPQUEUE Create();

/* Delete a queue */
void Delete(LPQUEUE);

/* Prints a queue */
void PrintQueue(LPQUEUE);


/*
 * Queue Operations:
 * 
 * Enqueue(): Insert element into queue at read
 * Dequeue(): Remove element from queue from front
 * Peek():    Display value of element at top
 * IsEmpty(): Check if the queue is empty
 * IsFull():  Check if the queue is full
 * 
 */


/* Enqueue int into stack pointed by LPQUEUE */
void Enqueue (LPQUEUE, int);

/* Dequeue int from stack pointed by LPQUEUE */
int  Dequeue  (LPQUEUE);

/* Peek int from stack pointed by LPQUEUE */
int  Peek (LPQUEUE);

/* returns 1 if stack pointed by LPQUEUE is empty, otherwise 0 */
int  IsEmpty (LPQUEUE);

/* returns 1 if stack pointed by LPQUEUE is full, otherwise 0 */
int  IsFull  (LPQUEUE);


