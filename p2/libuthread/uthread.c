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
	queue_t joiners;
} TCB;

int add_joiner(int obs, int sub);
void notify_joiners(int tid);
void collect(int tid);
void switch_thread(TCB* prev, TCB* next);
int init(uthread_func_t func, void*arg);
int init_tcb(TCB** tcb, int index, State s);
int init_ctx(TCB* tcb, uthread_func_t func, void* arg);

queue_t ready;
TCB threads[USHRT_MAX];
int num_threads;
static uthread_t curr_id;

void uthread_yield(void)
{
	preempt_enable();
	TCB* prev = &threads[curr_id];
	State s = prev->state;
	if(s == READY || s == RUNNING) {
		prev->state = READY;
		queue_enqueue(ready, prev);
	}

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
		init(NULL, NULL);
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

	notify_joiners(curr_id);

	uthread_yield();
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

	while(threads[tid].state != ZOMBIE) {
		threads[curr_id].state = BLOCKED;
		add_joiner(tid, curr_id); // let tid know curr_id is waiting
		uthread_yield();
	}

	if(retval) {
		*retval = threads[tid].retval;
	}

	collect(tid);

	return 0;
}

int add_joiner(int obs, int sub) {
	TCB* obs_tcb = &threads[obs];
	TCB* sub_tcb = &threads[sub];
	if(obs_tcb->joiners == NULL)
		obs_tcb->joiners = queue_create();
	return queue_enqueue(obs_tcb->joiners, sub_tcb);
}

void notify_joiners(int tid) {
	TCB* exited = &threads[tid];
	TCB* joiner = NULL;
	while(-1 != queue_dequeue(exited->joiners, (void**) &joiner)) {
		joiner->state = READY;
		queue_enqueue(ready, joiner);
	}
}

void collect(int tid) {

	TCB* td = &threads[tid];
	td->state = UNINITIALIZED;
	td->being_joined = false;
	preempt_disable();
	uthread_ctx_destroy_stack(td->ctx->uc_stack.ss_sp);
	preempt_enable();
}

void switch_thread(TCB* prev, TCB* next) {
	// Get next thread to run
	int retval = queue_dequeue(ready, (void**) &next);

	if(retval != -1) {
		curr_id = next->TID;
		next->state = RUNNING;
		preempt_disable();
		uthread_ctx_switch(prev->ctx, next->ctx);
		preempt_enable();
	}
}

int init(uthread_func_t func, void*arg) {
	preempt_start();
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

int init_tcb(TCB** tcb, int index, State s) {
	TCB *temp = &threads[index];
	if(tcb == NULL)
		return -1;
	temp->TID = index;
	temp->state = s;
	*tcb = temp;
	return 0;
}

int init_ctx(TCB* tcb, uthread_func_t func, void* arg) {
	preempt_disable();
	void* stack = uthread_ctx_alloc_stack();
	if(stack == NULL) // stack failed to allocate
		return -1;
	tcb->ctx = (uthread_ctx_t*) malloc(sizeof(uthread_ctx_t));
	if(!tcb->ctx) // ctx failed to allocate
		return -1;
	int retval = uthread_ctx_init(tcb->ctx, stack, func, arg);
	preempt_enable();
	return retval;
}
