#include <stdio.h>

#include "sll.h"

int main(void)
{
    int option, data, key;
    LPNODE head = NULL;

    do
    {
        printf("\n\n---- Singly Linked List ---- \n");
        printf("0. Create Linked List \n");
        printf("1. Insert at Head \n");
        printf("2. Insert at Tail \n");
        printf("3. Insert after Node \n");
        printf("4. Delete at Head \n");
        printf("5. Delete at Tail \n");
        printf("6. Delete Node \n");
        printf("7. Delete Linked List \n");
        printf("8. Display List \n");
        printf("9. Exit \n");
        printf("? ");
        scanf("%d", &option);
        printf("\n");

        switch (option)
        {
        case 0:
            head = Create();
            break;

        case 1:
            printf("Enter Value: ");
            scanf("%d", &data);
            head = InsertHead(head, data);
            PrintList(head);
            break;

        case 2:
            printf("Enter Value: ");
            scanf("%d", &data);
            head = InsertTail(head, data);
            PrintList(head);
            break;

        case 3:
            printf("Enter Value: ");
            scanf("%d", &data);
            printf("Enter Node to insert after: ");
            scanf("%d", &key);
            head = InsertAfter(head, data, key);
            PrintList(head);
            break;

        case 4:
            head = DeleteHead(head);
            PrintList(head);
            break;

        case 5:
            head = DeleteTail(head);
            PrintList(head);
            break;

        case 6:
            printf("Enter Value: ");
            scanf("%d", &data);
            head = DeleteNode(head, data);
            PrintList(head);
            break;

        case 7:
            head = Delete(head);
            PrintList(head);
            break;

        case 8:
            PrintList(head);
            break;

        case 9:
            printf("Exiting..\n");
            break;

        default:
            printf("Invalid Option!\n");
            break;
        }

    } while (option != 9);

    /* delete linked list before exiting */
    head = Delete(head);

    return (0);
}
