#ifndef SORTER_SERVER_H
#define SORTER_SERVER_H

typedef
struct _node
{
	char ** data;
	struct _node * next;
}
Node;



void mergesort(Node ** database, int columnHeader, int dataType);
Node * numSortedMerge(Node * a, Node * b, int columnIndex);
Node * strSortedMerge(Node * a, Node * b, int columnIndex);
void FrontBackSplit (Node * headNode, Node ** frontNode, Node ** backNode);

#endif
