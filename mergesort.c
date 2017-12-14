#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sorter_server.h"

void mergesort(Node ** headNode, int columnIndex, int dataType)
{
	//printf("SUP BRO, I MADE IT INTO MERGESORT!! WHATTUP?\n");
	Node * head = *headNode;
	Node * a;
	Node * b;

	//Base Case
	if ((head == NULL) || (head->next == NULL))
	{
		//printf("GUESS I WAS NULL AFTERALL, WHO KNEW\n");
		return;
	}

	//Split the list into 'a' and 'b' sublists
	
	FrontBackSplit(head, &a, &b);

	//printf("frontbacksplit ok\n");

	//Recursively sort the sublists
	
	//printf("a->data: %s\tb->data: %s\n", a->data[columnIndex], b->data[columnIndex]);
	mergesort(&a, columnIndex, dataType);
	mergesort(&b, columnIndex, dataType);

	//merge the two sorted lists into one complete sorted list
	
	if(dataType == 1)
	{
		*headNode = numSortedMerge(a, b, columnIndex);	//was head = (returned unsorted list missing the last entry)
	}else{
		*headNode = strSortedMerge(a, b, columnIndex);	//with change, returns only one row but it is the correct first row when sorted
	}
	//printf("Sorted merge ok\n");
}

Node* numSortedMerge(Node * a, Node * b, int columnIndex)
{

	Node * result; // = (Node *)malloc(sizeof(Node)); //was NULL
	
	//printf("Before base case check!");

	//Base Case
		
	if (a == NULL)
	{
		result = b;
		return result;		//added return to stop function front quitting here and just printing the one row of data, now seg faults below
	}else if (b == NULL)
	{
		result = a;
		return result;
	}
	
	//printf("a: %s vs b: %s\n", a->data[columnIndex], b->data[columnIndex]);
	//compare the data from each list and merge
	
	int aval = atoi(a->data[columnIndex]);
	int bval = atoi(b->data[columnIndex]);
	//printf("aval = %d, bval = %d\n", aval, bval);
	if (aval <= bval)
	{
	//	printf("a smaller than b\n");
		result = a;
		//result->next = (Node *)malloc(sizeof(Node));
		
		if (b != NULL && a->next != NULL)
		{
			result->next = numSortedMerge(a->next, b, columnIndex);
		}
		else
		{
			result->next = numSortedMerge(NULL, b, columnIndex);
		}

	}else{
	//	printf("b smaller than a\n");
		result = b;
	//	printf("result = b\n");
		//result->next = (Node *)malloc(sizeof(Node));
		/*if ( result->next == NULL)
		{
			printf("error allocating mem for result->next\n");
		}*/
		
		if (a != NULL && b->next != NULL)
		{
			result->next = numSortedMerge(a, b->next, columnIndex);
		}
		else
		{
	//		printf("something was NULL afterall\n");
			result->next = numSortedMerge(a, NULL, columnIndex);
		}
	}	
	//printf("result: %s\n", result->data[columnIndex]);	
	return result;
	
}

Node* strSortedMerge(Node * a, Node * b, int columnIndex)
{

	Node * result; // = (Node *)malloc(sizeof(Node));// NULL;

	//Base Case
	if (a == NULL)
	{
		return(b);
	}
	else if (b == NULL)
	{
		return(a);
	}

	//compare the data from each list and merge
	
	if (strcmp(a->data[columnIndex], b->data[columnIndex])<= 0)
	{

		result = a;
		//result->next = (Node *)malloc(sizeof(Node));
		if (a->next != NULL)
		{
			result->next = strSortedMerge(a->next, b, columnIndex);
		}
		else
		{
			result->next = strSortedMerge(NULL, b, columnIndex);
		}
	}else{
	
		result = b;
		//result->next = (Node *)malloc(sizeof(Node));
		if (b->next != NULL)
		{
			result->next = strSortedMerge(a, b->next, columnIndex);
		}
		else
		{
			result->next = strSortedMerge(a, NULL, columnIndex);
		}
	}

	return (result);
}

//Split the list into 2 separate lists

void FrontBackSplit(Node * headNode, Node ** frontNode, Node ** backNode)
{
	if(headNode == NULL || headNode->next == NULL)
	{
		*frontNode = headNode;
		*backNode = NULL;
		return;
	}else if(headNode->next->next == NULL)
	{
		*frontNode = headNode;
		*backNode = headNode->next;
		headNode->next = NULL;
		return;
	}else{
		
		Node * ptr = headNode;
		int count = 0;
		while( ptr != NULL)
		{
			count++;
			ptr = ptr -> next;
		}
	//	printf("count = %d\n", count);

		int midpoint = count/2;
	//	printf("midpoint = %d\n", midpoint);
		//Node * front = headNode;
		*frontNode = headNode;
		Node * back = headNode;
		int i;
		for(i = 0; i < midpoint && back != NULL ; i++)
		{
	//		printf("split ptr moved\n");
			back = back->next;
		}
		
	//	printf("frontNode: %s\n", headNode->data[3]);
		*backNode = back->next;
		//printf("backNode: %s\n", back->next->data[3]);
		back->next = NULL;	//separate the 2 lists
	}
}

