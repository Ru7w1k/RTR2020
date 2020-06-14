#include <stdio.h>
#include <stdlib.h>

#include "sll.h"

void InsertHead(LPNODE *head, int data)
{
	/* Create Node */
	LPNODE node = (LPNODE)malloc(sizeof(NODE));
	node->iData = data;
	node->next = node;

	/* Append at Head */
	if (*head)
	{
		node->next = (*head);
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

	/* Append at Tail */
	if (!(*head))
	{
		head = &node;
	}
	else
	{
		LPNODE tmp = node;
		while (tmp->next != NULL)
		{
			tmp = tmp->next;
		}

		tmp->next = node;
	}
}


void DeleteHead(LPNODE *head)
{
	LPNODE tmp;

	/* Head is NULL i.e. link list is empty */
	if (!(*head)) return;

	/* store current head in temp, and delete current head */
	tmp = *head;

	/* move head to next of head */
	(*head) = (*head)->next();

	/* free old head node */
	free(*tmp); 
}


void DeleteTail(LPNODE *head)
{
	LPNODE tailNode;

	/* Head is NULL i.e. link list is empty */
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
	/* Head is NULL i.e. link list is empty */
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

