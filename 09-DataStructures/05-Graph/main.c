#include <stdio.h>

#include "graph.h"

int main(void)
{
    int option, data, key;
    LPGRAPH graph = NULL;

    do
    {
        printf("\n\n---- Graph ---- \n");
        printf("0. Create Graph \n");
        printf("1. BFS \n");
        printf("2. DFS \n");
        printf("3. Print Graph \n");
        printf("4. Exit \n");
        printf("? ");
        scanf("%d", &option);
        printf("\n");

        switch (option)
        {
        case 0:
            printf("Enter no of vertices: ");
            scanf("%d", &data);
            graph = Create(data);
            break;

        case 1:
            BFS(graph);
            break;

        case 2:
            DFS(graph);
            break;

        case 3:
            PrintGraph(graph);
            break;

        case 4:
            printf("Exiting..\n");
            break;

        default:
            printf("Invalid Option!\n");
            break;
        }

    } while (option != 4);

    /* delete linked list before exiting */
    Delete(graph);

    return (0);
}
