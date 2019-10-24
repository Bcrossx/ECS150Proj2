#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

typedef struct Linked {
	struct Linked* next;
	struct Linked* prev;
	void* data;
} LinkedEl;

struct queue {
	LinkedEl* top;
	LinkedEl* bottom;
	int length;
};

queue_t queue_create(void)
{
	queue_t q = (queue_t) malloc(sizeof(struct queue));
	q->top = NULL;
	q->bottom = NULL;
	q->length = 0;
	return q;
}

int queue_destroy(queue_t queue)
{
	if(queue == NULL)
		return -1;
	if(queue->length > 0)
		return -1;
	free(queue);
	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	if(queue == NULL || data == NULL)
		return -1;
	LinkedEl* el = (LinkedEl*) malloc(sizeof(LinkedEl));
	if(el == NULL) // malloc failed
		return -1;
	el->next = queue->top;
	el->data = data;
	queue->top = el;
	queue->length++;
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if(queue == NULL || data == NULL)
		return -1;
	if(queue->length == 0)
		return -1;
	*data = queue->bottom->data;
	queue->bottom = queue->bottom->prev;
	free(queue->bottom->next);
	queue->bottom->next = NULL; // make sure new bottom doesn't point to DQ'd elmt
	queue->length--;
	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	if(queue == NULL || data == NULL)
		return -1;
	if(queue->length == 0)
		return -1;
	// At this pt we know that the queue exists and is not empty
	// So we'll search for the item now
	LinkedEl* curr = queue->bottom;
	do {
			// If current item doesn't contain wanted data, move on
			if(curr->data != data) {
				curr = curr->prev;
				continue;
			}
			// Need to treat top/bottom elements differently
			if(curr == queue->top) {
				curr->next->prev = NULL;
				queue->top = curr->next;
			} else if(curr == queue->bottom) {
				curr->prev->next = NULL;
				queue->bottom = curr->prev;
			} else {
				curr->prev->next = curr->next;
				curr->next->prev = curr->prev;
			}
			free(curr);
			queue->length--;
			return 0;
	} while(curr != queue->top);
	return -1;
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
	if(queue == NULL || func == NULL)
		return -1;
	// Iterate and Apply function
	int retval;
	LinkedEl* curr = queue->bottom;
	do {
		retval = (*func)(curr->data, arg);
		if(retval == 1)
			break;
		curr = curr->prev;
	}	while (retval == 0 && curr != queue->top);
	if(retval == 1 && data != NULL){
		*data = curr->data;
	}
	return 0;
}

int queue_length(queue_t queue)
{
	return queue->length;
}
