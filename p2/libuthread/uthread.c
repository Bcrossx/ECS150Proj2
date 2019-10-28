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
	READY,
	RUNNING,
	BLOCKED,
	ZOMBIE
} State;

typedef struct{
	uthread_t TID;
	State state;
	bool is_joining;
	uthread_ctx_t* ctx;
	int retval;
} TCB;

void init();

queue_t ready;
TCB threads[USHRT_MAX];
int num_threads;
uthread_t curr_id;

void uthread_yield(void)
{

	/* TODO Phase 2 */
}

uthread_t uthread_self(void)
{
	return curr_id;
}

int uthread_create(uthread_func_t func, void *arg)
{
	if(num_threads == 0)
		init();
	if(num_threads == USHRT_MAX)
		return -1;

	int index = num_threads - 1;
	TCB* tcb = &threads[index];
	if(tcb == NULL)
		return -1;
	tcb->TID = index;
	tcb->state = READY;

	void* stack = uthread_ctx_alloc_stack();
	if(stack == NULL) // stack failed to allocate
		return -1;
	int retval = uthread_ctx_init(tcb->ctx, stack, func, arg);
	if(retval == -1) // context failed to initialize
		return -1;
	retval = queue_enqueue(ready, tcb);
	if(retval == -1) // failed to enqueue
		return -1;

	return tcb->TID;
}

void uthread_exit(int retval)
{
	threads[curr_id].retval = retval;
	threads[curr_id].state = ZOMBIE;
}

int uthread_join(uthread_t tid, int *retval)
{
	if(tid == 0 || tid == curr_id) {
		return -1;
	}
	State curr = threads[tid].state;
	if(curr == READY || curr == ZOMBIE) // thread cannot be found
		return -1;
	if(threads[tid].is_joining)
		return -1;
	threads[tid].is_joining = true;

	while(threads[tid].state != ZOMBIE); // block while waiting

	*retval = threads[tid].retval;


	/* TODO Phase 3 */
	return 0;
}

void init() {
	ready = queue_create();
	num_threads = 1;
}
