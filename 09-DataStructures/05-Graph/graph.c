/*
 * Graph
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"

/* Create graph from number of nodes */
LPGRAPH Create(int nodes)
{
	/* create graph struct */
	LPGRAPH graph = (LPGRAPH)malloc(sizeof(GRAPH));
	graph->vertices = nodes;

	/* allocate memory for adjecancy list */
	graph->adjList = (LPNODE*)malloc(sizeof(LPNODE) * nodes);

	/* iterate over each vertex to get its connectios */
	for (int i = 0; i < nodes; i++)
	{
		/* create node for this vertex */
		graph->adjList[i] = (LPNODE)malloc(sizeof(NODE));
		graph->adjList[i]->next = NULL;
		graph->adjList[i]->vertex = i;

		/* initialize the last with this node */
		LPNODE last = graph->adjList[i];

		int edges = 0;
		printf("\nEnter number of connections from node %d: ", i);
		scanf("%d", &edges);

		for (int j = 0; j < edges; j++)
		{
			int connectedNode = 0;
			printf("\nEnter the %d connection from node %d: ", j, i);
			scanf("%d", &connectedNode);

			/* create a node for new connection */
			LPNODE node = (LPNODE)malloc(sizeof(NODE));
			node->next = NULL;
			node->vertex = connectedNode;

			/* append connection at the end */
			last->next = node;

			/* update last node */
			last = node;
		}
	}
	return(graph);
}


/* breadth first search */
void BFS(LPGRAPH g)
{
	int queue[100],visited[100], front = -1, rear = -1;
	memset(visited, 0, sizeof(visited));

	printf("\nBFS:");
	
	rear++;
	queue[rear] = g->adjList[0]->vertex;
	front = 0;
	visited[queue[front]] = 1;
	printf(" %d", queue[front]);

	while (front <= rear)
	{
		LPNODE pNode = g->adjList[queue[front++]]->next;
		while(pNode)
		{
			/* if the vertex is not visited */
			if (visited[pNode->vertex] != 1)
			{
				/* print vertex */
				printf(" %d", pNode->vertex);

				/* mark it as visited */
				visited[pNode->vertex] = 1;

				/* insert into queue */
				queue[++rear] = pNode->vertex;
			}

			/* move to next node */
			pNode = pNode->next;
		}
	}
};


/* depth first search */
void DFS(LPGRAPH g)
{
	int stack[100],visited[100], top = -1;
	memset(visited, 0, sizeof(visited));

	printf("\nDFS:");
	
	stack[++top] = g->adjList[0]->vertex;

	while (top != -1)
	{
		/* visit stack top vertex if not visited */
		if( visited[stack[top]] != 1)
		{
			visited[stack[top]] = 1;
			printf(" %d", stack[top]);
		}

		/* push all neighbours of that vertex into stack */
		LPNODE pNode = g->adjList[stack[top--]]->next;

		while(pNode)
		{
			/* if the vertex is not visited */
			if (visited[pNode->vertex] != 1)
			{
				/* push into stack */
				stack[++top] = pNode->vertex;
			}                                                                                      

			/* move to next node */
			pNode = pNode->next;
		}
	}
};


/* delete graph */
void Delete(LPGRAPH g) 
{
	for (int i = 0; i < g->vertices; g++)
	{
		LPNODE pNode = g->adjList[i];
		while(pNode)
		{
			LPNODE temp = pNode;
			pNode = pNode->next;
			free(temp);
		}
		g->adjList[i] = NULL;
	}
};


/* print graph */
void PrintGraph(LPGRAPH g)
{
	printf("\n\n --- GRAPH ---");

	for(int i = 0; i < g->vertices; i++)
	{
		LPNODE pNode = g->adjList[i];

		printf("\nVertex %d: ", pNode->vertex);

		pNode = pNode->next;
		while(pNode)
		{
			printf("%d  ", pNode->vertex);
			pNode = pNode->next;
		}
	}
}


