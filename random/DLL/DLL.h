//written by Tyler Raborn

#ifndef DLL_H
#define DLL_H

typedef struct Node
{
	char* data;
	struct Node* next;
	struct Node* prev;

} Node;

typedef struct DLL
{
	struct Node* head;
	struct Node* tail;

} DLL;

int isFull;

//void initialize(DLL*);
void insert_front(DLL*, char*);
void insert_back(DLL*, char*);
void printList(DLL*);
void pop_front(DLL*);
void pop_back(DLL*);
void delete_node(DLL*, char*);
int find(DLL*, char*);
void perror(const char*);


#endif
