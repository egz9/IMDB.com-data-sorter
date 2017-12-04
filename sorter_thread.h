//SYSTEMS PROGRAMMING PROJECT 2

#ifndef SORTER_H
#define SORTER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <pthread.h>

extern char * category;
extern pthread_mutex_t csv_node_lock;
typedef struct{
	char *wholerow; // The whole row as a single string.
	char *key; //The string of the column category.
}rowtype;

typedef struct thread_args{
	char * path;
	char * entry_name;
}Thread_Args;

typedef struct thread_node{
	pthread_t tid;
	struct thread_node * next;
}Thread_Node;

typedef struct csv_Node {//new addition 11/27
	rowtype** data;
	int length;
	int numMerges;
	struct csv_Node* next;
}csvNode;

extern csvNode* head;

Thread_Node * addtid(pthread_t tid, Thread_Node * headThread);

void * enter_directory(Thread_Args * args);


//Suggestion: prototype a mergesort function
void recordCpy(rowtype * dest, rowtype * src);

void merge(int low, int mid, int high, rowtype** arr, char typeFlag);

// SAM this is mergesort. Do not use merge function
// sort uses merge so its all covered
void sort(int low, int high, rowtype ** arr, char typeFlag);

int is_csv(char * s1);

void addpid(pid_t p);

void clearArray();

char getTypeFlag(char* columnToken);

int getSortingColumnCount(char* catSortString);

void * sortcsv(Thread_Args * args);

csvNode* forceMerge( csvNode* front, char typeFlag);

csvNode* addToMaster(csvNode* front, rowtype** newData, int length, char typeFlag );

csvNode* tryToMerge( csvNode* front, char typeFlag);
#endif
