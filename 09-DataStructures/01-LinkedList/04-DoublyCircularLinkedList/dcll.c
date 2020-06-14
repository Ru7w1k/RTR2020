#include <stdio.h>

#include "dcll.h"

void InsertHead(LPNODE *head, int data)
{
	/* Create Node */
	LPNODE node = (LPNODE)malloc(sizeof(NODE));
	node->iData = data;
	node->next = node;
	node->prev = node;

	/* Append at Head */
	if (*head)
	{
		node->prev = (*head)->prev;
		node->next = (*head);
		(*head)->prev->next = node;
		(*head)->prev = node;
	}

	/* Update the NEW head */
	*head = node;

}

void InsertTail(LPNODE *head, int data)
{
	/* Create Node */
	LPNODE node = (LPNODE)malloc(sizeof(NODE));
	node->iData = data;
	node->next = node;
	node->prev = node;

	/* Append at Tail */
	if (!(*head))
	{
		head = &node;
	}
	else
	{
		node->prev = (*head)->prev;
		node->next = (*head);
		(*head)->prev->next = node;
		(*head)->prev = node;
	}

}


void DeleteHead(LPNODE *head)
{
	LPNODE newHead;

	if (!(*head)) return;

	/* break links with head node */
	(*head)->next->prev = (*head)->prev;
	(*head)->prev->next = (*head)->next;

	newHead = (*head)->next;

	if (*head == newHead)
	{
		free(*head);
		*head = NULL;
	}
	else
	{
		free(*head);
		*head = newHead;
	}

}


void DeleteTail(LPNODE *head)
{
	LPNODE tailNode;

	if (!(*head)) return;

	tailNode = (*head)->prev;

	/* break links of tail node */
	tailNode->prev->next = (*head);
	(*head)->prev = tailNode->prev;


	if (*head == tailNode)
	{
		free(tailNode);
		*head = NULL;
	}
	else
	{
		free(tailNode);
	}

}


void PrintList(LPNODE *head)
{
	if (!(*head))
	{
		printf("\n\nLinked List: <empty>");
		return;
	}

	LPNODE node = (*head);

	/* Traverse the full list */
	printf("\n\nLinked List: ");
	do
	{
		printf("%d ", node->iData);
		node = node->next;
	} while (node != (*head));

}

