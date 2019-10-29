#include <assert.h>
#include <signal.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>

#include "context.h"
#include "preempt.h"
#include "queue.h"
#include "uthread.h"

typedef enum {
	UNINITIALIZED,
	READY,
	RUNNING,
	BLOCKED,
	ZOMBIE
} State;

typedef struct{
	uthread_t TID;
	State state;
	bool being_joined;
	uthread_ctx_t* ctx;
	int retval;
} TCB;

int init(uthread_func_t func, void*arg);
void switch_thread(TCB* prev, TCB* next);
int init_tcb(TCB** tcb, int index, State s);
int init_ctx(TCB* tcb, uthread_func_t func, void* arg);

queue_t ready;
TCB threads[USHRT_MAX];
int num_threads;
static uthread_t curr_id;

void uthread_yield(void)
{
	TCB* prev = &threads[curr_id];
	prev->state = READY;
	queue_enqueue(ready, prev);

	TCB *next = NULL;
	switch_thread(prev, next);
}

uthread_t uthread_self(void)
{
	return curr_id;
}

int uthread_create(uthread_func_t func, void *arg)
{
	if(num_threads == 0)
		init(func, arg);
	if(num_threads == USHRT_MAX)
		return -1;

	int index = num_threads;
	TCB* tcb;
	if(init_tcb(&tcb, index, READY) == -1)
		return -1;
	if(init_ctx(tcb, func, arg) == -1) // context failed to initialize
		return -1;
	if(queue_enqueue(ready, tcb) == -1) // failed to enqueue
		return -1;
	num_threads++;

	if(num_threads == 2) // need to initiate first context switch
		uthread_yield();
	return tcb->TID;
}

void uthread_exit(int retval)
{
	TCB* prev = &threads[curr_id];
	prev->state = ZOMBIE;
	prev->retval = retval;

	TCB *next = NULL;
	switch_thread(prev, next); // Grab next thread & switch context
}

int uthread_join(uthread_t tid, int *retval)
{
	if(tid == 0 || tid == curr_id) {
		return -1;
	}
	State curr = threads[tid].state;
	if(curr == UNINITIALIZED) // thread cannot be found
		return -1;
	if(threads[tid].being_joined)
		return -1;
	threads[tid].being_joined = true;

	while(threads[tid].state != ZOMBIE); // block while waiting

	*retval = threads[tid].retval;
	/* TODO Phase 3 */
	return 0;
}

int init(uthread_func_t func, void*arg) {
	ready = queue_create();
	num_threads = 1;

	// Add main thread to thread list
	TCB* tcb = NULL;
	if(-1 == init_tcb(&tcb, 0, RUNNING))
		return -1;
	if(-1 == init_ctx(tcb, func, arg))
		return -1;

	return 0;
}

void switch_thread(TCB* prev, TCB* next) {
	// Get next thread to run
	int retval = queue_dequeue(ready, (void**) &next);
	assert(retval != -1);

	curr_id = next->TID;
	next->state = RUNNING;
	uthread_ctx_switch(prev->ctx, next->ctx);
}

int init_tcb(TCB** tcb, int index, State s) {
	TCB *temp = &threads[index];
	assert(tcb != NULL);
//	if(tcb == NULL)
//		return -1;
	temp->TID = index;
	temp->state = s;
	*tcb = temp;
	return 0;
}

int init_ctx(TCB* tcb, uthread_func_t func, void* arg) {
	void* stack = uthread_ctx_alloc_stack();
	if(stack == NULL) // stack failed to allocate
		return -1;
	tcb->ctx = (uthread_ctx_t*) malloc(sizeof(uthread_ctx_t));
	if(!tcb->ctx) // ctx failed to allocate
		return -1;
	return uthread_ctx_init(tcb->ctx, stack, func, arg);
}
