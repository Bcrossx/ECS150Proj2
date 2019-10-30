/*
 * Simple hello world test
 *
 * Tests the creation of a single thread and its successful return.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <uthread.h>
#include <preempt.h>

int hello(void* arg)
{
	printf("Hello world!\n");
	return 24;
}

int main(void)
{
	preempt_disable();
	uthread_t tid;
	int retval;

	tid = uthread_create(hello, NULL);
	assert(-1 != uthread_join(tid, &retval));

	return 0;
}
