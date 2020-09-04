// Red-Black Tree

#include <stdio.h>
#include <stdlib.h>

#include "bst.h"

Node *FindMin(Node *node);
void ReplaceNode(Node *node, Node* child);

void InsertRecursive(Node *root, Node *node);
void InsertRepairTree(Node *node);

Node *Insert(Node *root, Node *node);
void InsertCase1(Node *);
void InsertCase2(Node *);
void InsertCase3(Node *);
void InsertCase4(Node *);
void InsertCase4Step2(Node *);

Node *DeleteOneChild(Node *node);
void DeleteCase1(Node *);
void DeleteCase2(Node *);
void DeleteCase3(Node *);
void DeleteCase4(Node *);
void DeleteCase5(Node *);
void DeleteCase6(Node *);


Node *FindMin(Node *node) {
	Node *current = node;
	while (current->left != LEAF && current->left != NULL)
		current = current->left;

	return current;
}

Node *Find(Node *root, int data) {

	if (root == NULL || root == LEAF)
		return NULL;
	
	if (data < root->data) {
		return Find(root->left, data);
	}
	else if (data > root->data) {
		return Find(root->right, data);
	}
	else { /* Found! */
		return root;
	}

	/* SHOULD NEVER REACH HERE */
	return NULL;
}

void ReplaceNode(Node *node, Node *child) {
	child->parent = node->parent;

	if (node->parent)
	{
		if (node == node->parent->left)
			node->parent->left = child;
		else
			node->parent->right = child;
	}
}

Node *Insert(Node *root, int data) {
	
	Node *temp = Find(root, data);
	if (temp != NULL)
		return root;


	Node *node = (Node *)malloc(sizeof(Node));
	node->data = data;
	node->left = NULL;
	node->right = NULL;
	node->parent = NULL;

	return Insert(root, node);
}

Node *Insert(Node *root, Node *node) {
	// Insert the new node into the current tree
	InsertRecursive(root, node);

	// Repair the tree, if required
	InsertRepairTree(node);

	// Find the new Root node
	root = node;
	while (Parent(root) != NULL) {
		root = Parent(root);
	}
	return root;
}

void InsertRecursive(Node *root, Node *node) {
	// Recursively descend the tree until a leaf is found
	if (root != NULL && node->data < root->data) {
		if (root->left != LEAF) {
			InsertRecursive(root->left, node);
			return;
		}
		else {
			root->left = node;
		}
	}
	else if (root != NULL) {
		if (root->right != LEAF) {
			InsertRecursive(root->right, node);
			return;
		}
		else {
			root->right = node;
		}
	}
	
	// Insert node
	node->parent = root;
	node->left = LEAF;
	node->right = LEAF;
	node->Color = RED;
}

void InsertRepairTree(Node *node) {
	if (Parent(node) == NULL) {
		InsertCase1(node);
	}
	else if (Parent(node)->Color == BLACK) {
		InsertCase2(node);
	}
	else if (Uncle(node) != NULL && Uncle(node)->Color == RED) {
		InsertCase3(node);
	}
	else {
		InsertCase4(node);
	}
}

void InsertCase1(Node *node) {
	if (Parent(node) == NULL) {
		node->Color = BLACK;
	}
}

void InsertCase2(Node *node) {
	return;
}

void InsertCase3(Node *node) {
	Parent(node)->Color = BLACK;
	Uncle(node)->Color = BLACK;
	GrandParent(node)->Color = RED;
	InsertRepairTree(GrandParent(node));
}

void InsertCase4(Node *node) {
	Node *p = Parent(node);
	Node *g = GrandParent(node);

	if (node == p->right && p == g->left) {
		RotateLeft(p);
		node = node->left;
	}
	else if (node == p->left && p == g->right) {
		RotateRight(p);
		node = node->right;
	}

	InsertCase4Step2(node);
}

void InsertCase4Step2(Node *node) {
	Node *p = Parent(node);
	Node *g = GrandParent(node);

	if (node == p->left) {
		RotateRight(g);
	}
	else {
		RotateLeft(g);
	}

	p->Color = BLACK;
	g->Color = RED;
}

Node *Delete(Node *node, int data) {
	Node *root = node;

	if (data < node->data && node->left != LEAF && node->left != NULL)
	{
		return Delete(node->left, data);		
	}

	if (data > node->data && node->right != LEAF && node->right != NULL)
	{
		return Delete(node->right, data);
	}

	if (data != node->data)	{
		while (root->parent != NULL) {
			root = root->parent;
		}
		return root;
	}
	// got the node with data
	if (node->left != NULL && node->right != NULL && node->right != LEAF && node->left != LEAF) /* both children present */
	{		
		Node *minNode = FindMin(node->right);
		node->data = minNode->data;
		return Delete(node->right, minNode->data);
	}
	else /* only one child present */
	{
		return DeleteOneChild(node);
	}
		
}

Node *DeleteOneChild(Node *node) {
	// node has at most one non-leaf child
	Node *root = node;
	Node *child = node->right == LEAF ? node->left : node->right;
	if (child == LEAF) {

		/* Root Node */
		if (node->parent == NULL) { 
			return NULL;
		}

		if (node == node->parent->left) {
			node->parent->left = LEAF;
		}
		else {
			node->parent->right = LEAF;
		}

		while (root->parent != NULL)
			root = root->parent;

		free(node);
		node = NULL;
		return root;
	}

	ReplaceNode(node, child);
	if (node->Color == BLACK)
	{
		if (child->Color == RED)
		{
			child->Color = BLACK;
		}
		else
			DeleteCase1(child);
	}

	root = child;
	while (root->parent != NULL)
		root = root->parent;

	free(node);
	node = NULL;
	
	return root;
}

void DeleteCase1(Node *node) {
	if (node->parent != NULL)
		DeleteCase2(node);
}

void DeleteCase2(Node *node) {
	Node *s = Sibling(node);

	if (s->Color == RED)
	{
		node->parent->Color = RED;
		s->Color = BLACK;

		if (node == node->parent->left)
			RotateLeft(node->parent);

		else
			RotateRight(node->parent);
	}

	DeleteCase3(node);
}

void DeleteCase3(Node *node) {
	Node *s = Sibling(node);

	if (node->parent->Color == BLACK && s->Color == BLACK &&
		s->left->Color == BLACK && s->right->Color == BLACK)
	{
		s->Color = RED;
		DeleteCase1(node->parent);
	}
	else
	{
		DeleteCase4(node);
	}
}

void DeleteCase4(Node *node) {
	Node *s = Sibling(node);

	if (node->parent->Color == RED && s->Color == BLACK &&
		s->left->Color == BLACK && s->right->Color == BLACK)
	{
		s->Color = RED;
		node->parent->Color = BLACK;
	}
	else
	{
		DeleteCase5(node);
	}
}

void DeleteCase5(Node *node) {
	Node *s = Sibling(node);

	if (s->Color == BLACK)
	{
		if ((node == node->parent->left) &&
			(s->right->Color == BLACK) &&
			(s->left->Color == RED))
		{
			s->Color = RED;
			s->left->Color = BLACK;
			RotateRight(s);
		}
		else if ((node == node->parent->right) &&
			(s->left->Color == BLACK) &&
			(s->right->Color == RED))
		{
			s->Color = RED;
			s->right->Color = BLACK;
			RotateLeft(s);
		}
	}
	DeleteCase6(node);
}

void DeleteCase6(Node *node) {
	Node *s = Sibling(node);

	s->Color = node->parent->Color;
	node->parent->Color = BLACK;

	if (node == node->parent->left)
	{
		s->right->Color = BLACK;
		RotateLeft(node->parent);
	}
	else
	{
		s->left->Color = BLACK;
		RotateRight(node->parent);
	}
}

void Print(Node *root, int level) {
	if (root == NULL) {
		printf("\nEmpty Tree!");
		return;
	}


	if (root->left) {
		Print(root->left, level + 1);
	}

	if (root->data != 0)
	{
		printf(" [%d ", root->data);

		if (root->Color == BLACK) printf("Black ");
		else printf("Red ");
		printf("%d]", level);
	}

	if (root->right) {
		Print(root->right, level + 1);
	}
}

std::queue<int> InOrder(Node *root, int level)
{
	static std::queue<int> qNodes;
	if (level == 0) {
		while (!qNodes.empty()) qNodes.pop();
	}

	if (root->left != LEAF)
		qNodes = InOrder(root->left, level + 1);

	qNodes.push(root->data);

	if (root->right != LEAF)
		qNodes = InOrder(root->right, level + 1);

	return qNodes;
}

std::queue<int> PreOrder(Node *root, int level)
{
	static std::queue<int> qNodes;
	if (level == 0) {
		while (!qNodes.empty()) qNodes.pop();
	}

	qNodes.push(root->data);

	if (root->left != LEAF)
		qNodes = PreOrder(root->left, level + 1);

	if (root->right != LEAF)
		qNodes = PreOrder(root->right, level + 1);

	return qNodes;
}

std::queue<int> PostOrder(Node *root, int level)
{
	static std::queue<int> qNodes;
	if (level == 0) {
		while (!qNodes.empty()) qNodes.pop();
	}

	if (root->left != LEAF)
		qNodes = PostOrder(root->left, level + 1);

	if (root->right != LEAF)
		qNodes = PostOrder(root->right, level + 1);
	
qNodes.push(root->data);

	return qNodes;
}



//int main(void)
//{
//	Node *root = NULL;
//
//	Node *node = (Node *)malloc(sizeof(Node));
//	node->data = 10;
//
//	root = Insert(root, node);
//	printf("\n");
//	Print(root, 0);
//
//	node = (Node *)malloc(sizeof(Node));
//	node->data = 20;
//
//	root = Insert(root, node);
//	printf("\n");
//	Print(root, 0);
//
//	node = (Node *)malloc(sizeof(Node));
//	node->data = 70;
//
//	root = Insert(root, node);
//	printf("\n");
//	Print(root, 0);
//
//	node = (Node *)malloc(sizeof(Node));
//	node->data = 50;
//
//	root = Insert(root, node);
//	printf("\n");
//	Print(root, 0);
//
//	node = (Node *)malloc(sizeof(Node));
//	node->data = 5;
//
//	root = Insert(root, node);
//	printf("\n");
//	Print(root, 0);
//
//	root = Delete(root, 20);
//	printf("\n");
//	Print(root, 0);
//
//	root = Delete(root, 10);
//	printf("\n");
//	Print(root, 0);
//
//	root = Delete(root, 50);
//	printf("\n");
//	Print(root, 0);
//
//	root = Delete(root, 5);
//	printf("\n");
//	Print(root, 0);
//
//	root = Delete(root, 70);
//	printf("\n");
//	Print(root, 0);
//}




