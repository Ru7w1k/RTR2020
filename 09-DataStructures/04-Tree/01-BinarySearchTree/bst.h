#pragma once

typedef struct _Node {
	int data;
	struct _Node *left;
	struct _Node *right;
} Node;

void Print(Node *root, int level);
Node *Insert(Node *root, int node);
Node *Delete(Node *node, int data);
Node *Find(Node *node, int data);

void InOrder(Node *root, int level);
void PreOrder(Node *root, int level);
void PostOrder(Node *root, int level);


