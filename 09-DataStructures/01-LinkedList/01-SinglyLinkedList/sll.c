/*
 * Singly Linked List
 */

#include <stdio.h>
#include <stdlib.h>

#include "sll.h"


/* Create Linked List */
LPNODE Create()
{
	int value = 0;
	LPNODE head = NULL;

	printf("\nEnter -1 to stop loop");
	while (1)
	{
		printf("\nEnter Value: ");
		scanf("%d", &value);

		if (value == -1) 
			break;

		head = InsertTail(head, value);
		PrintList(head);
	}

	return head;
}


/* Insert node at Head position (start) */
LPNODE InsertHead(LPNODE head, int data)
{
	/* create Node */
	LPNODE node = (LPNODE)malloc(sizeof(NODE));
	node->iData = data;
	node->next = NULL;

	/* append at Head */
	node->next = head;

	head = node;
	
	return head;
}


/* Insert node at Tail position (end) */
LPNODE InsertTail(LPNODE head, int data)
{
	/* create Node */
	LPNODE node = (LPNODE)malloc(sizeof(NODE));
	node->iData = data;
	node->next = NULL;

	/* empty linked list */
	if (head == NULL)
	{
		return node;
	}

	/* traverse till the end of linked list */
	LPNODE tmp = head;
	while (tmp->next != NULL)
	{
		tmp = tmp->next;

	};

	/* append the new node at the end */
	tmp->next = node;

	return head;
}


/* Insert the data after key value */
LPNODE InsertAfter(LPNODE head, int data, int key)
{
	int found = 0;

	/* create Node */
	LPNODE node = (LPNODE)malloc(sizeof(NODE));
	node->iData = data;
	node->next = NULL;

	/* empty linked list */
	if (head == NULL)
	{
		free(node);
		return head;
	}

	/* traverse till the key is found */
	LPNODE tmp = head;
	while (tmp != NULL) 
	{
		/* if key found */
		if (tmp->iData == key) 
		{
			found = 1;
			break;
		}

		tmp = tmp->next;
	};

	/* append the new node after the key node */
	if (found)
	{
		node->next = tmp->next;
		tmp->next = node;
	}
	else
	{
		printf("\nThe key %d is not present in linked list..", key);
		/* free the node */
		free(node);	
	}

	return head;
}


/* Delete node at Head (start) */
LPNODE DeleteHead(LPNODE head)
{
	LPNODE tmp;

	/* head is NULL i.e. link list is empty */
	if (head == NULL) return NULL;

	/* store current head in temp, and delete current head */
	tmp = head;

	/* move head to next of head */
	head = head->next;

	/* free old head node */
	free(tmp); 

	return head;
}


/* Delete node at tail (end) */
LPNODE DeleteTail(LPNODE head)
{
	/* head is NULL i.e. link list is empty */
	if (head == NULL) return NULL;

	/* traverse till the end of linked list */
	LPNODE tmp = head;
	LPNODE prev = head;
	
	while (tmp->next != NULL)
	{
		prev = tmp;
		tmp = tmp->next;

	};

	/* break the link between tail and its previous node */
	prev->next = NULL;

	/* if tail node is the head, then set head to NULL */
	if (tmp == head) head = NULL;

	/* free the node */
	free(tmp);

	return head;
}


/* Delete node with given value*/
LPNODE DeleteNode(LPNODE head, int key)
{
	/* empty linked list */
	if (head == NULL)
	{
		return head;
	}

	/* traverse till the key is found */
	LPNODE tmp = head;
	LPNODE prev = head;

	while (tmp != NULL && tmp->iData != key)
	{
		prev = tmp;
		tmp = tmp->next;
	}

	/* if key found delete that node*/
	if (tmp != NULL) 
	{
		/* tmp is not Head */
		if (tmp != head)
		{
			prev->next = tmp->next;
		}
		/* tmp is Head */
		else
		{
			head = head->next;
		}
		
		free(tmp);
	}
	else
	{
		printf("\nThe key %d is not present in linked list..", key);
	}
	

	return head;
}


/* Print the entire linked list */
void PrintList(LPNODE head)
{
	/* head is NULL i.e. link list is empty */
	if (head == NULL)
	{
		printf("\n\nLinked List: <empty>");
		return;
	}

	LPNODE node = head;

	/* traverse the full list */
	printf("\n\nLinked List: ");
	do
	{
		printf("%d ", node->iData);
		node = node->next;

	} while (node != NULL);
}


/* Delete entire linked list */
LPNODE Delete(LPNODE head)
{
	/* traverse and delete all elements from linked list */
	while (head != NULL)
	{
		head = DeleteHead(head);
	}

	/* head must be NULL now */
	return head;
}

