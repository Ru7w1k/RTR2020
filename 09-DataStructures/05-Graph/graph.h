/*
 * Graph
 */

/* Building Block of Graph */
typedef struct _NODE {

	int vertex;
	struct _NODE *next;

} NODE, *LPNODE;

/* Graph */
typedef struct _GRAPH {

	int vertices;
	LPNODE *adjList;

} GRAPH, *LPGRAPH;

/*
 * NODE is typedef of _NODE
 * LPNODE is typedef of _NODE *, so it is of type pointer to struct _NODE
 * /

/* Linked List Operations:
 * 
 * All functions take LPNODE as first parameter, which represents the HEAD of linked list.
 * Almost all functions also return LPNODE, because when we insert at head or delete the head node,
 * a new node will act as HEAD node, that new HEAD node is returned by those functions.
 * 
 */

LPGRAPH Create(int);

void BFS(LPGRAPH);
void DFS(LPGRAPH);

void Delete(LPGRAPH);

void PrintGraph(LPGRAPH);
