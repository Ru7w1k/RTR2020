/*
 * Singly Linked List
 */

/* Building Block of Linked List */
typedef struct _NODE {

	int iData;
	struct _NODE *next;

} NODE, *LPNODE;

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

LPNODE Create();

LPNODE InsertHead(LPNODE, int);
LPNODE InsertTail(LPNODE, int);
LPNODE InsertAfter(LPNODE, int, int);

LPNODE DeleteHead(LPNODE);
LPNODE DeleteTail(LPNODE);
LPNODE DeleteNode(LPNODE, int);

LPNODE Delete(LPNODE);

void PrintList(LPNODE);

