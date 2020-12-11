//Michael Lederer 
//UIN: 677140523
//NetID: mleder3
//CS 251 Project 2 Pt I
#include <stdio.h>
#include <stdlib.h>
#include "sq.h"

/**
* file:  sq.c
*
* description:  simple implementation of the
*		service queue ADT as specified
*		in sq.h
*
*		(Believed to be) functionally correct,
*		but doe not meet all of the runtime
*		requirements specified in sq.h
*
*		Acts as a client of the linked list
*		ADT previously developed.
*/
typedef struct node {
	int id;
	int inQ;
	struct node *next;
	struct node *prev;
}NODE;

typedef struct list
{
	NODE *front;
	NODE *back;
	int size;
} LIST;


struct service_queue
{
	LIST *queue;
	LIST *bucket;
	NODE **arr;
	int arrSize;
};

/**
* Function: sq_create()
* Description: creates and intializes an empty service queue.
* 	It is returned as an SQ pointer.
*/
SQ * sq_create() {
	SQ *q;
	int i;
	//initialize the bucket
	q = malloc(sizeof(SQ));

	q->bucket = (LIST*)malloc(sizeof(LIST));
	q->queue = (LIST*)malloc(sizeof(LIST));

	q->bucket->front = NULL;
	q->bucket->back = NULL;
	q->bucket->size = 0;
	//initialize the service queue
	q->queue->size = 0;
	//q = malloc(sizeof(SQ));
	q->arr = (NODE **)malloc(10 * sizeof(NODE*));
	q->arrSize = 10;
	for (i = 0; i < q->arrSize; i++)
	{
		//Point the array elements equal to empty nodes, corresponding to the inidicies
		q->arr[i] = malloc(sizeof(NODE));
		q->arr[i]->id = i;
		q->arr[i]->inQ = 0;

		//If the bucket is empty, assign the first node to the front of the list
		if (q->bucket->front == NULL)
		{
			q->bucket->front = q->arr[i];
			q->bucket->back = q->arr[i];
			q->arr[i]->next = NULL;
			q->arr[i]->prev = NULL;
			q->bucket->size++;
		}
		//If its not empty (i.e. after the first pass of the loop), only change the back
		else
		{
			q->bucket->back = q->arr[i];
			q->bucket->back->next = NULL;
			q->bucket->back->prev = q->arr[i - 1];
			q->arr[i - 1]->next = q->bucket->back;
			q->bucket->size++;
		}
	}

	return q;
}

/**
* Function: sq_free()
* Description:  see sq.h
*
* RUNTIME REQUIREMENT:  O(N_t)  where N_t is the number of buzzer
*	IDs that have been used during the lifetime of the
*	service queue; in general, at any particular instant
*	the actual queue length may be less than N_t.
*
*	[See discussion of in header file]
*
* RUNTIME ACHIEVED:  ???
*
*/
void sq_free(SQ *q) {
	int i;
	//traverse the array map and free each node
	for (i = 0; i < q->arrSize; i++)
	{
		q->arr[i]->next = NULL;
		q->arr[i]->prev = NULL;
		free(q->arr[i]);
	}
}

/**
* Function: sq_display()
* Description:  see sq.h
*
* REQUIRED RUNTIME:  O(N)  (N is the current queue length).
* ACHIEVED RUNTIME:  O(N)  YAY!!
*/
void sq_display(SQ *q) {
	printf("current-queue contents:\n    ");
	int i;

	//create a node to traverse the queue so it only prints nodes in the list 
	//in the order of the list
	NODE *temp = malloc(sizeof(NODE));
	temp = q->queue->front;
	while (temp != NULL)
	{
		if (temp->next == NULL)
		{
			printf(" %d ", temp->id);
			break;
		}
		printf(" %d ", temp->id);
		temp = temp->next;
	}


	printf("\n");
}

/**
* Function: sq_length()
* Description:  see sq.h
*
* REQUIRED RUNTIME:  O(1)
* ACHIEVED RUNTIME:  ???
*/
int  sq_length(SQ *q) {
	return q->queue->size;
}

/**
* Function: sq_give_buzzer()
* Description:  see sq.h
*
* REQUIRED RUNTIME:  O(1)  ON AVERAGE or "AMORTIZED":  in other words, if
*		there have been M calls to sq_give_buzzer, the total
*		time taken for those M calls is O(M).
*
*		An individual call may therefore not be O(1) so long
*		as when taken as a whole they average constant time.
*
*		(Hopefully this reminds you of an idea we employed in
*		the array-based implementation of the stack ADT).
*
* ACHIEVED RUNTIME:  ???
*/
int  sq_give_buzzer(SQ *q) {
	//Check if the queue is empty
	if (q->queue->size == 0)
	{
		//Set the queue size to 1 and change the front
		q->queue->size = 1;

		//Use an int to hold the ID of the front of the 
		//bucket so as not to lose it when changing the front of the bucket
		int i;
		i = q->bucket->front->id;

		//Change the front of the bucket
		q->bucket->front = q->bucket->front->next;
		q->bucket->front->prev = NULL;

		//Add the new buzzer to the front of the queue
		q->queue->front = q->arr[i];
		q->queue->back = q->arr[i];
		q->queue->front->next = NULL;
		q->arr[i]->inQ = 1;
		q->bucket->size--;
		return q->queue->front->id;
	}
	else
	{
		//If the front of the bucket is less than the greatest magnitude ID taken (reusable)
		if (q->bucket->front->next != NULL)
		{
			//Use an int to hold the ID of the front of the 
			//bucket so as not to lose it when changing the front of the bucket
			int i;
			i = q->bucket->front->id;

			//Move the front of the bucket to the back of the queue
			q->bucket->front = q->bucket->front->next;
			q->bucket->front->prev = NULL;
			//Reallign the pointers for the back of the queue
			q->queue->back->next = q->arr[i];
			q->arr[i]->prev = q->queue->back;
			q->queue->back = q->arr[i];

			q->queue->back->next = NULL;
			q->arr[i]->inQ = 1;
			q->queue->size++;
			q->bucket->size--;
			return q->queue->back->id;
		}
		else
		{
			//Move the last node from the front of the bucket 
			//to the back of the queue
			q->queue->back->next = q->bucket->front;
			q->bucket->front->prev = q->queue->back;
			q->queue->back = q->bucket->front;
			q->queue->back->next = NULL;
			q->bucket->front = NULL;
			q->bucket->back = NULL;
			q->queue->back->inQ = 1;
			q->queue->size++;

			//We now need to double the size of the array map
			//Or we will be drawing from nodes that haven't been created yet
			int i;
			q->arr = realloc(q->arr, q->arrSize * 2 * sizeof(NODE*));
			for (i = q->arrSize; i < q->arrSize * 2; i++)
			{
				//Point the array elements equal to empty nodes, corresponding to the inidicies
				q->arr[i] = malloc(sizeof(NODE));
				q->arr[i]->id = i;
				q->arr[i]->inQ = 0;

				//If the bucket is empty (first pass)
				if (q->bucket->front == NULL)
				{
					q->bucket->front = q->arr[i];
					q->arr[i]->prev = NULL;
					q->arr[i]->next = NULL;
					q->bucket->back = q->arr[i];
				}
				//If the bucket has at least 1 buzzer in it
				else
				{
					q->bucket->back->next = q->arr[i];
					q->bucket->back = q->arr[i];
					q->bucket->back->next = NULL;
				}
			}
			q->arrSize *= 2;
			return q->queue->back->id;
		}

	}
}

/**
* function: sq_seat()
* description:  see sq.h
*
* REQUIRED RUNTIME:  O(1)
* ACHIEVED RUNTIME:  ???
*/
int sq_seat(SQ *q) {

	//Take the node from the front of the queue
	//and move it to the front of the bucket
	int i = q->queue->front->id;
	q->queue->front = q->queue->front->next;
	q->arr[i]->next = q->bucket->front;
	q->bucket->front->prev = q->arr[i];
	q->bucket->front = q->arr[i];
	q->arr[i]->inQ = 0;
	return q->bucket->front->id;
}



/**
* function: sq_kick_out()
*
* description:  see sq.h
*
* REQUIRED RUNTIME:  O(1)
* ACHIEVED RUNTIME:  ???
*/
int sq_kick_out(SQ *q, int buzzer) {
	//Check that the buzzer exists or that it is in the queue
	if (buzzer > q->arrSize || q->arr[buzzer]->inQ == 0)
	{
		return 0;
	}
	else
	{
		//If the buzzer is at the front of the queue
		if (q->arr[buzzer] == q->queue->front)
		{
			q->queue->front = q->queue->front->next;
			q->arr[buzzer]->next = q->bucket->front;
			q->bucket->front->prev = q->arr[buzzer];
			q->bucket->front = q->arr[buzzer];
			q->arr[buzzer]->inQ = 0;
			q->queue->size--;

			//If the buzzer is 0, we have to return the node, 
			//otherwise it will "return 0" and cause an error
			if (q->arr[buzzer]->id == 0)
			{
				return q->arr[buzzer];
			}
			return q->arr[buzzer]->id;
		}
		//If the buzzer it at the back of the queue
		else if (q->arr[buzzer] == q->queue->back)
		{
			q->queue->back = q->queue->back->prev;
			q->bucket->front->prev = q->arr[buzzer];
			q->arr[buzzer]->next = q->bucket->front;
			q->bucket->front = q->arr[buzzer];
			q->queue->back->next = NULL;
			q->arr[buzzer]->inQ = 0;
			q->queue->size--;
			//If the buzzer is 0, we have to return the node, 
			//otherwise it will "return 0" and cause an error
			if (q->arr[buzzer]->id == 0)
			{
				return q->arr[buzzer];
			}
			return q->arr[buzzer]->id;
		}
		//If the buzzer is in the middle of the queue
		else
		{
			//Reallign pointers of nodes behind and ahead of the buzzer 
			//to be removed
			q->arr[buzzer]->prev->next = q->arr[buzzer]->next;
			q->arr[buzzer]->next->prev = q->arr[buzzer]->prev;

			//Move buzzer to be removed to the bucket
			q->arr[buzzer]->next = q->bucket->front;
			q->arr[buzzer]->prev = NULL;
			q->bucket->front = q->arr[buzzer];
			q->arr[buzzer]->inQ = 0;
			q->queue->size--;
			//If the buzzer is 0, we have to return the node, 
			//otherwise it will "return 0" and cause an error
			if (q->arr[buzzer]->id == 0)
			{
				return q->arr[buzzer];
			}
			return q->arr[buzzer]->id;
		}
	}
}

/**
* function:  sq_take_bribe()
* description:  see sq.h
*
* REQUIRED RUNTIME:  O(1)
* ACHIEVED RUNTIME:  ???
*/
int sq_take_bribe(SQ *q, int buzzer) {
	//If the buzzer is at the back of the list
	if (q->queue->back == q->arr[buzzer])
	{
		q->queue->back = q->queue->back->prev;
		q->queue->front->prev = q->arr[buzzer];
		q->arr[buzzer]->next = q->queue->front;
		q->queue->front = q->arr[buzzer];
		q->queue->back->next = NULL;
		return q->arr[buzzer]->id;

	}
	//If its not at the back and its in the queue, 
	//it either exists at the front or in the middle
	else if (q->arr[buzzer] != NULL && q->arr[buzzer]->inQ == 1)
	{
		//Reallign pointers of nodes behind and ahead of the buzzer 
		//to be removed
		q->arr[buzzer]->prev->next = q->arr[buzzer]->next;
		q->arr[buzzer]->next->prev = q->arr[buzzer]->prev;

		//Move the buzzer to the front
		q->arr[buzzer]->next = q->queue->front;
		q->queue->front->prev = q->arr[buzzer];
		q->queue->front = q->arr[buzzer];
		q->arr[buzzer]->prev = NULL;
		return q->arr[buzzer]->id;
	}
	else
	{
		return 0;
	}
}
