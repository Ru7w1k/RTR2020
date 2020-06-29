#include <stdio.h>

#include "stack.h"

int main(void)
{
    int option, data;
    LPSTACK pStack = NULL;

    do
    {
        printf("\n\n---- Stack (using array) ---- \n");
        printf("0. Create Stack \n");
        printf("1. Push \n");
        printf("2. Pop \n");
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
            /* if already stack is created, delete is before creating new stack */
            if (pStack) Delete(pStack);
            
            pStack = Create();
            break;

        case 1:
            printf("Enter Value: ");
            scanf("%d", &data);
            Push(pStack, data);
            PrintStack(pStack);
            break;

        case 2:
            data = Pop(pStack);
            printf("\nPopped value: %d", data);
            PrintStack(pStack);
            break;

        case 3:
            data = Peek(pStack);
            printf("\nPeek value: %d", data);
            PrintStack(pStack);
            break;

        case 4:
            if (IsFull(pStack))
                printf("\nThe Stack if FULL!");
            else
                printf("\nThe Stack if NOT FULL!");
            break;

        case 5:
            if (IsEmpty(pStack))
                printf("\nThe Stack if EMPTY!");
            else
                printf("\nThe Stack if NOT EMPTY!");
            break;

        case 6:
            printf("Exiting..\n");
            break;

        default:
            printf("Invalid Option!\n");
            break;
        }

    } while (option != 6);

    /* delete stack before exiting */
    Delete(pStack);
    pStack = NULL;

    return (0);
}
