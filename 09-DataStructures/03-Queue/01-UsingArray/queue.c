#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

/* Creates a queue */
LPQUEUE Create()
{
    LPQUEUE pQueue = (LPQUEUE)malloc(sizeof(QUEUE));
    pQueue->front = -1;
    pQueue->rear  = -1;
    return pQueue;
}

/* Delete a queue */
void Delete(LPQUEUE pQueue)
{
    if (pQueue == NULL)
    {
        printf("\nNULL Queue pointer!");
        return;
    }

    free(pQueue);
}


/* Enqueue int into queue pointed by LPQUEUE */
void Enqueue(LPQUEUE pQueue, int data)
{
    if (pQueue == NULL)
    {
        printf("\nNULL Queue pointer!");
        return;
    }

    if (IsFull(pQueue))
    {
        printf("\nQueue is FULL, cannot enqueue()...");
    }
    else
    {
        /* if it is the first element, set front and rear both */
        if (pQueue->front == -1 && pQueue->rear == -1)
        {
            pQueue->front = 0;
            pQueue->rear = 0;
        }
        else
        {
            pQueue->rear++;
        }
        pQueue->data[pQueue->rear] = data;
    }
}

/* Dequeue int from queue pointed by LPQUEUE */
int Dequeue(LPQUEUE pQueue)
{
    if (pQueue == NULL)
    {
        printf("\nNULL Queue pointer!");
        return -1;
    }

    if (IsEmpty(pQueue))
    {
        printf("\nQueue is EMPTY, cannot dequeue()...");
        return -1;
    }
    else
    {
        int frontData = pQueue->data[pQueue->front];
        pQueue->front++;
        return frontData;
    }
}

/* Peek int from queue pointed by LPQUEUE */
int Peek(LPQUEUE pQueue)
{
    if (pQueue == NULL)
    {
        printf("\nNULL Queue pointer!");
        return -1;
    }

    if (IsEmpty(pQueue))
    {
        printf("\nQueue is EMPTY, cannot peek()...");
        return -1;
    }
    else
    {
        int frontData = pQueue->data[pQueue->front];
        return frontData;
    }
}

/* returns 1 if queue pointed by LPQUEUE is empty, otherwise 0 */
int IsEmpty(LPQUEUE pQueue)
{
    if (pQueue == NULL)
    {
        printf("\nNULL Queue pointer!");
        return -1;
    }

    if (pQueue->front == -1 || pQueue->front > pQueue->rear)
        return 1;

    return 0;
}

/* returns 1 if queue pointed by LPQUEUE is full, otherwise 0 */
int IsFull(LPQUEUE pQueue)
{
    if (pQueue == NULL)
    {
        printf("\nNULL Queue pointer!");
        return -1;
    }

    if (pQueue->rear == QUEUE_SIZE - 1)
        return 1;

    return 0;
}

/* Prints a queue */
void PrintQueue(LPQUEUE pQueue)
{
    if (pQueue == NULL)
    {
        printf("\nNULL Queue pointer!");
        return;
    }

    printf("\n\nQueue: ");

    if (IsEmpty(pQueue))
    {
        printf("<empty>");
    }
    else
    {
        for (int i = pQueue->front; i <= pQueue->rear; i++)
        {
            printf("%d  ", pQueue->data[i]);
        }
    }
    printf("\n\n");
}


