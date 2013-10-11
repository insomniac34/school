//doubly-Linked List demonstration in C
//written by Tyler Raborn

#include <cstdio>
#include <cstdlib>
#include "DLL.h"

void initialize(DLL* listPtr)
{
	isFull = 0;
}

void insert_front(DLL* curList, char* newdata)
{
	struct Node* newNode;
	newNode = (Node*)malloc(sizeof(Node));
	newNode->data = newdata;
	newNode->next = NULL;
	newNode->prev = NULL;
	
	if (isFull==0) //if list is empty
	{
	    (curList->head) = newNode;
		(curList->tail) = newNode;

		isFull = 1;
	}
	else //list is not empty
	{
		curList->head->next = newNode; //current head's next pointer points to newNode
		newNode->prev = curList->head; //previous pointer of the new node points to current head
		curList->head = newNode; //head now points to newNode
	}
}

void insert_back(DLL* curList, char* newdata)
{
	struct Node* newNode;
	newNode = (Node*)malloc(sizeof(Node));
	newNode->data = newdata;
	newNode->next = NULL;
	newNode->prev = NULL;

	if (isFull==0) //if list is empty
	{
		//curList = (DLL*)malloc(sizeof(DLL));
		
		//initialize(curList);
		(curList->head) = newNode;
		(curList->tail) = newNode;

		isFull = 1;
	}
	else //list is not empty
	{
		curList->tail->prev = newNode; //current tail's prev pointer points to newNode
		newNode->next = curList->tail; //->next pointer of newNode points to the current tail.
		curList->tail = newNode; //tail now points to newNode
	}
}

void printList(DLL* curList)
{
	int ans;
	printf("Please enter '0' to iterate from front-to-back, or '1' to iterate from back-to-front:\n");
	scanf("%d", &ans);
	struct Node* iterator;

	if (ans==0)
	{
		iterator = curList->head;
		while(iterator!=NULL)
		{
			printf("%s\n", &(*(iterator->data)));
			iterator = iterator->prev;
		}
	}
	else if (ans==1)
	{
		iterator = curList->tail;
		while (iterator!=NULL)
		{
			printf("%s\n", &(*(iterator->data)));
			iterator = iterator->next;
		}
	}
}

void pop_front(DLL* curList)
{
	Node* temp;
	temp = curList->head->prev;
	free(curList->head);
	curList->head = temp; 
	curList->head->next = NULL;
}

void pop_back(DLL* curList)
{
	Node* temp;
	temp = curList->tail->next;
	free(curList->tail);
	curList->tail = temp;
	curList->tail->prev = NULL;
}

void delete_node(DLL* curList, char* target)
{
	if (isFull!=0)
	{
		Node* temp;
		temp = curList->head;
		while (temp!=NULL)
		{
			if (strcmp(temp->data, target)==0)
			{
				//target found; proceed w/ deletion.
				if ((temp->prev!=NULL) && (temp->next!=NULL))
				{
					temp->prev->next = temp->next;
					temp->next->prev = temp->prev;
				}
				else if (temp->prev==NULL)
				{
					curList->tail = temp->next;
					curList->tail->prev = NULL;
				}
				else if (temp->next==NULL)
				{
					curList->head = temp->prev;
					curList->head->next = NULL;
				}
				else
				{
					isFull = 0;
				}
				free(temp);
				break;
			}
			temp = temp->prev;
		}
	}
}

int find(DLL* curList, char* target) //returns 1 if target found, else returns 0
{
	if (isFull!=0)
	{
		Node* temp;
		temp = curList->head;
		while (temp!=NULL)
		{
			if (strcmp(temp->data, target)==0)
			{
				return 1;
			}
			temp = temp->prev;
		}
		return 0;
	}
	return 0;
}

void error(const char* msg)
{
	perror(msg);
	exit(-1);
}
