#include <stdio.h>

#include "queue.h"

int main(void)
{
    int option, data;
    LPQUEUE pQueue = NULL;

    do
    {
        printf("\n\n---- Queue (using array) ---- \n");
        printf("0. Create Queue \n");
        printf("1. Enqueue \n");
        printf("2. Dequeue \n");
        printf("3. Peek \n");
        printf("4. IsFull \n");
        printf("5. IsEmpty \n");
        printf("6. Exit \n");
        printf("? ");
        scanf("%d", &option);
        printf("\n");

        switch (option)
        {
        case 0:
            /* if already queue is created, delete is before creating new queue */
            if (pQueue) Delete(pQueue);
            
            pQueue = Create();
            break;

        case 1:
            printf("Enter Value: ");
            scanf("%d", &data);
            Enqueue(pQueue, data);
            PrintQueue(pQueue);
            break;

        case 2:
            data = Dequeue(pQueue);
            printf("\nDequeue value: %d", data);
            PrintQueue(pQueue);
            break;

        case 3:
            data = Peek(pQueue);
            printf("\nPeek value: %d", data);
            PrintQueue(pQueue);
            break;

        case 4:
            if (IsFull(pQueue))
                printf("\nThe queue is FULL!");
            else
                printf("\nThe queue is NOT FULL!");
            break;

        case 5:
            if (IsEmpty(pQueue))
                printf("\nThe queue is EMPTY!");
            else
                printf("\nThe queue is NOT EMPTY!");
            break;

        case 6:
            printf("Exiting..\n");
            break;

        default:
            printf("Invalid Option!\n");
            break;
        }

    } while (option != 6);

    /* delete queue before exiting */
    Delete(pQueue);
    pQueue = NULL;

    return (0);
}
