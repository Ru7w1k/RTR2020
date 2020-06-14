
/* Basic Unit of Linked List */
typedef struct _NODE {

	int iData;
	struct _NODE *next;

} NODE, *LPNODE;

/* Linked List Operations */
void InsertHead(LPNODE*, int);
void InsertTail(LPNODE*, int);
void DeleteHead(LPNODE*);
void DeleteTail(LPNODE*);
void PrintList(LPNODE*);

