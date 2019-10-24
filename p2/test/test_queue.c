#include <assert.h>
#include "queue.h"

// Implemented in order of dependence
void test_create() {
	assert(queue_create() != NULL && "test_create failed");
}

void test_destroy() {
	// Setup
	queue_t q = queue_create();

	// Test
	queue_destroy(q);
	assert(q == NULL && "test_destroy failed");
}

void test_nq() {
	// Setup
	queue_t q = queue_create();
	int data = 5;

	// Test
	queue_enqueue(q, &data);
	assert(q->top->data == data && "test_nq failed");

	// Teardown
	queue_destroy(q);
}

void test_dq() {
	// Setup
	queue_t q = queue_create();
	int data = 5, *store;
	queue_enqueue(q, &data);

	// Test
	queue_dequeue(q, &store);
	assert(*store == data && "test_dq failed");

	// Teardown
	queue_destroy(q);
}

void test_length() {
	// Setup
	queue_t q = queue_create();
	int data = 5;
	queue_enqueue(q, &data);

	// Test
	assert(queue_length(q) == 1 && "test_length failed");

	// Teardown
	queue_dequeue(q, &data);
	queue_destroy(q);
}

void test_delete() {
	// Setup
	queue_t q = queue_create();
	int data = 5;
	queue_enqueue(q, &data);

	// Test
	queue_delete(q, &data);
	assert(queue_length(q) == 0 && "test_delete failed");

	// Teardown
	queue_destroy(q);
}

int __test_iterate_fxn__(int data, void* arg) {
	return val+1;
}

void test_iterate() {
	// Setup
	queue_t q = queue_create();
	int data = 5, *store;
	queue_enqueue(q, &data);

	// Test
	queue_iterate(q, &__test_iterate_fxn__, NULL, &store);
	assert(*store == data && "test_iterate failed")

	// Teardown
	queue_delete(q, &data);
	queue_destroy(q);
}
