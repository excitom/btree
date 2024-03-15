/**
 * Inserting a key on a B-tree
 *
 * This program accepts an input file contining one integer per line
 * and inserts each one into a B-tree. When all the numbers are inserted
 * the tree is traversed and the numbers printed. They should be in ascending
 * numerical order.
 *
 * Tom Lang 3/2024
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX 8
#define MIN 4

typedef struct btreeNode {
	int item[MAX + 1], count;
	struct btreeNode *link[MAX + 1];
} btreeNode;

btreeNode *root;

// Node creation
btreeNode *
createNode (int item, btreeNode *child)
{
	btreeNode *n;
	n = (btreeNode *) malloc (sizeof (btreeNode));
	n->item[1] = item;
	n->count = 1;
	n->link[0] = root;
	n->link[1] = child;
	return n;
}

// Insert
void
insertValue (int item, int pos, btreeNode *node, btreeNode *child)
{
	int j = node->count;
	while (j > pos) {
		node->item[j + 1] = node->item[j];
		node->link[j + 1] = node->link[j];
		j--;
	}
	node->item[j + 1] = item;
	node->link[j + 1] = child;
	node->count++;
}

// Split node
void splitNode (int item, int *pval, int pos, btreeNode *node, btreeNode *child, btreeNode **newNode)
{
	int median, j;

	if (pos > MIN) {
		median = MIN + 1;
	} else {
		median = MIN;
	}

	*newNode = (btreeNode *) malloc (sizeof (btreeNode));
	j = median + 1;
	while (j <= MAX) {
		(*newNode)->item[j - median] = node->item[j];
		(*newNode)->link[j - median] = node->link[j];
		j++;
	}
	node->count = median;
	(*newNode)->count = MAX - median;

	if (pos <= MIN) {
			insertValue (item, pos, node, child);
	} else {
		insertValue (item, pos - median, *newNode, child);
	}
	*pval = node->item[node->count];
	(*newNode)->link[0] = node->link[node->count];
	node->count--;
}

// Set the value of node
int setNodeValue (int item, int *pval,	btreeNode *node, btreeNode **child)
{
	int pos;
	if (!node) {
		*pval = item;
		*child = NULL;
		return 1;
	}

	if (item < node->item[1]) {
		pos = 0;
	} else {
		for (pos = node->count; (item < node->item[pos] && pos > 1); pos--);
		if (item == node->item[pos]) {
			printf ("Duplicates not allowed\n");
			return 0;
		}
	}
	if (setNodeValue (item, pval, node->link[pos], child)) {
		if (node->count < MAX) {
			insertValue (*pval, pos, node, *child);
		} else {
			splitNode (*pval, pval, pos, node, *child, child);
			return 1;
		}
	}
	return 0;
}

// Insert the value
void
insertItem (int item)
{
	int flag, i;
	btreeNode *child;

	flag = setNodeValue (item, &i, root, &child);
	if (flag) {
		root = createNode (i, child);
	}
}

// Copy the successor
void
copySuccessor (btreeNode *myNode, int pos)
{
	btreeNode *dummy;
	dummy = myNode->link[pos];

	for (; dummy->link[0] != NULL;) {
		dummy = dummy->link[0];
	}
	myNode->item[pos] = dummy->item[1];
}

// Do rightshift
void
rightShift (btreeNode *myNode, int pos)
{
	btreeNode *x = myNode->link[pos];
	int j = x->count;

	while (j > 0) {
		x->item[j + 1] = x->item[j];
		x->link[j + 1] = x->link[j];
	}
	x->item[1] = myNode->item[pos];
	x->link[1] = x->link[0];
	x->count++;

	x = myNode->link[pos - 1];
	myNode->item[pos] = x->item[x->count];
	myNode->link[pos] = x->link[x->count];
	x->count--;
	return;
}

// Do leftshift
void leftShift (btreeNode *myNode, int pos)
{
	int j = 1;
	btreeNode *x = myNode->link[pos - 1];

	x->count++;
	x->item[x->count] = myNode->item[pos];
	x->link[x->count] = myNode->link[pos]->link[0];

	x = myNode->link[pos];
	myNode->item[pos] = x->item[1];
	x->link[0] = x->link[1];
	x->count--;

	while (j <= x->count) {
		x->item[j] = x->item[j + 1];
		x->link[j] = x->link[j + 1];
		j++;
	}
	return;
}

// Merge the nodes
void mergeNodes (btreeNode *myNode, int pos)
{
	int j = 1;
	btreeNode *x1 = myNode->link[pos], *x2 = myNode->link[pos - 1];

	x2->count++;
	x2->item[x2->count] = myNode->item[pos];
	x2->link[x2->count] = myNode->link[0];

	while (j <= x1->count) {
		x2->count++;
		x2->item[x2->count] = x1->item[j];
		x2->link[x2->count] = x1->link[j];
		j++;
	}

	j = pos;
	while (j < myNode->count) {
		myNode->item[j] = myNode->item[j + 1];
		myNode->link[j] = myNode->link[j + 1];
		j++;
	}
	myNode->count--;
	free (x1);
}

// Adjust the node
void
adjustNode (btreeNode *myNode, int pos)
{
	if (!pos) {
		if (myNode->link[1]->count > MIN) {
			leftShift (myNode, 1);
		} else {
			mergeNodes (myNode, 1);
		}
	} else {
		if (myNode->count != pos) {
			if (myNode->link[pos - 1]->count > MIN) {
				rightShift (myNode, pos);
			} else {
				if (myNode->link[pos + 1]->count > MIN) {
					leftShift (myNode, pos + 1);
				} else {
					mergeNodes (myNode, pos);
				}
			}
		} else {
			if (myNode->link[pos - 1]->count > MIN) {
				rightShift (myNode, pos);
			} else {
				mergeNodes (myNode, pos);
			}
		}
	}
}

// Traverse the tree
void traversal (btreeNode *myNode)
{
	int i;
	if (myNode) {
		for (i = 0; i < myNode->count; i++) {
			traversal (myNode->link[i]);
			printf ("%d\n", myNode->item[i + 1]);
		}
		traversal (myNode->link[i]);
	}
}

int
main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Please supply a file name\n");
		exit(1);
	}
	FILE *fh = fopen(argv[1], "r");
	if (!fh) {
		printf("Can't open file '%s'\n", argv[1]);
		exit(1);
	}

	char str[255];
	//for (int i = 1; i<=100; i++) {
		//fgets(str, sizeof(str), fh);
	while (fgets(str, sizeof(str), fh) != NULL) {
		insertItem(atoi(str));
	}

	traversal (root);
	fclose(fh);
}
