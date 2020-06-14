#include <stdio.h>

#include "sll.h"

int main(void)
{
    int option, data;
    LPNODE head = NULL;

    do
    {

        printf("\n\nSingly Linked List: \n");
        printf("1. Insert at Head \n");
        printf("2. Insert at Tail \n");
        printf("3. Delete at Head \n");
        printf("4. Delete at Tail \n");
        printf("5. Display List \n");
        printf("6. Exit \n");
        printf("? ");
        scanf("%d", &option);
        printf("\n");

        switch (option)
        {
        case 1:
            printf("Enter Value: ");
            scanf("%d", &data);
            InsertHead(&head, data);
            PrintList(&head);
            break;

        case 2:
            printf("Enter Value: ");
            scanf("%d", &data);
            InsertTail(&head, data);
            PrintList(&head);
            break;

        case 3:
            DeleteHead(&head);
            PrintList(&head);
            break;

        case 4:
            DeleteTail(&head);
            PrintList(&head);
            break;

        case 5:
            PrintList(&head);
            break;

        default:
            break;
        }
    } while (option != 6);

    return (0);
}
