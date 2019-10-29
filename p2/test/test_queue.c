#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
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

// Functions defined in order of dependence

void test_create() {
	printf("test_create\n");

	// Test
	assert(queue_create() != NULL);
}

void test_destroy_null() {
	printf("test_destroy_null\n");

	// Setup
	queue_t q = (queue_t) NULL;

	// Test
	assert(queue_destroy(q) == -1);
}

void test_destroy_not_empty() {
	printf("test_destroy_not_empty\n");

	// Setup
	queue_t q = queue_create();
	int data = 5;
	queue_enqueue(q, &data);

	// Test
	assert(queue_destroy(q) == -1);

	// Teardown
	queue_delete(q, &data);
	queue_destroy(q);
}

void test_destroy() {
	printf("test_destroy\n");

	// Setup
	queue_t q = queue_create();

	// Test
	assert(queue_destroy(q) == 0);

	// Other cases
	test_destroy_null();
	test_destroy_not_empty();
}

void test_nq_q_null() {
	printf("test_nq_q_null\n");

	// Setup
	queue_t q = (queue_t) NULL;
	int data = 5;

	// Test
	int retval = queue_enqueue(q, (void*) &data);
	assert(retval == -1);

	// Teardown
	queue_destroy(q);
}

void test_nq_data_null() {
	printf("test_nq_data_null\n");

	// Setup
	queue_t q = queue_create();

	// Test
	int retval = queue_enqueue(q, NULL);
	assert(retval == -1);

	// Teardown
	queue_destroy(q);
}

// Test enqueue
void test_nq() {
	printf("test_nq\n");

	// Setup
	queue_t q = queue_create();
	int data = 5;

	// Test
	queue_enqueue(q, (void*) &data);
	assert(*((int*)(q->top->data)) == data);

	// Teardown
	queue_destroy(q);

	// Other cases
	test_nq_q_null();
	test_nq_data_null();
}

void test_dq_q_null() {
	printf("test_dq_q_null\n");

	// Setup
	queue_t q = (queue_t) NULL;
	int data = 5, *store = &data;

	// Test
	int retval = queue_dequeue(q, (void**) &store);
	assert(retval == -1);

	// Teardown
	queue_destroy(q);
}

void test_dq_data_null() {
	printf("test_dq_data_null\n");

	// Setup
	queue_t q = queue_create();

	// Test
	int retval = queue_dequeue(q, (void**) NULL);
	assert(retval == -1);

	// Teardown
	queue_destroy(q);
}

void test_dq_empty() {
	printf("test_dq_empty\n");

	// Setup
	queue_t q = queue_create();
	int data = 5, *store = &data;

	// Test
	int retval = queue_dequeue(q, (void**) &store);
	assert(retval == -1);

	// Teardown
	queue_destroy(q);
}

// Test dequeue
void test_dq() {
	printf("test_dq\n");

	// Setup
	queue_t q = queue_create();
	int data = 5, temp = 0, *store = &temp;
	queue_enqueue(q, (void*) &data);

	// Test
	int retval = queue_dequeue(q, (void**) &store);
	assert(retval == 0);
	assert(*store == data);

	// Teardown
	queue_destroy(q);

	// Other cases
	test_dq_q_null();
	test_dq_data_null();
	test_dq_empty();
}

void test_length_q_null() {
	printf("test_length_q_null\n");

	// Setup
	queue_t q = (queue_t) NULL;

	// Test
	assert(queue_length(q) == -1);

	// Teardown
	queue_dequeue(q, (void**) NULL);
	queue_destroy(q);
}

void test_length() {
	printf("test_length\n");

	// Setup
	queue_t q = queue_create();
	int data = 5;
	queue_enqueue(q, (void*) &data);

	// Test
	assert(queue_length(q) == 1);

	// Teardown
	queue_dequeue(q, (void**) NULL);
	queue_destroy(q);

	// Other cases
	test_length_q_null();
}

void test_delete_q_null() {
	printf("test_delete_q_null\n");

	// Setup
	queue_t q = (queue_t) NULL;
	int data = 5;

	// Test
	int retval = queue_delete(q, (void*) &data);
	assert(retval == -1);

	// Teardown
	queue_destroy(q);
}

void test_delete_data_null() {
	printf("test_delete_data_null\n");

	// Setup
	queue_t q = queue_create();

	// Test
	int retval = queue_delete(q, (void*) NULL);
	assert(retval == -1);

	// Teardown
	queue_destroy(q);
}

void test_delete_not_found() {
	printf("test_delete_not_found\n");

	// Setup
	queue_t q = queue_create();
	int data = 5, search = 0;
	queue_enqueue(q, (void*) &data);

	// Test
	int retval = queue_delete(q, (void*) &search);
	assert(retval == -1);

	// Teardown
	queue_destroy(q);
}

void test_delete() {
	printf("test_delete\n");

	// Setup
	queue_t q = queue_create();
	int data = 5;
	queue_enqueue(q, (void*) &data);

	// Test
	int retval = queue_delete(q, (void*) &data);
	assert(retval == 0);
	assert(queue_length(q) == 0);

	// Teardown
	queue_destroy(q);

	// Other cases
	test_delete_q_null();
	test_delete_data_null();
	test_delete_not_found();
}

int __test_iterate_fxn__(void* data, void* arg) {
	*((int*) data) += 1;
	return 0;
}

int __test_iterate_func_stop_fxn__(void* data, void* arg) {
	*((int*) data) += 1;
	return 1;
}

void test_iterate_q_null() {
	printf("test_iterate_q_null\n");

	// Setup
	queue_t q = (queue_t) NULL;
	int data = 5, *store = &data;
	queue_func_t cb = (queue_func_t) &__test_iterate_fxn__;

	// Test
	int retval = queue_iterate(q, cb, NULL, (void**) &store);
	assert(retval == -1);

	// Teardown
	queue_destroy(q);
}

void test_iterate_func_null() {
	printf("test_iterate_func_null\n");

	// Setup
	queue_t q = queue_create();
	queue_func_t cb = (queue_func_t) NULL;

	// Test
	int retval = queue_iterate(q, cb, NULL, (void**) NULL);
	assert(retval == -1);

	// Teardown
	queue_destroy(q);
}

void test_iterate_func_stop() {
	printf("test_iterate_func_stop\n");

	// Setup
	queue_t q = queue_create();
	int data = 5;
	queue_enqueue(q, &data);
	queue_func_t cb = (queue_func_t) &__test_iterate_func_stop_fxn__;

	// Test
	int retval = queue_iterate(q, cb, NULL, (void**) NULL);
	assert(retval == 0);
	assert(data == 6);

	// Teardown
	queue_delete(q, &data);
	queue_destroy(q);
}

void test_iterate_func_stop_data_def() {
	printf("test_iterate_func_stop_data_def\n");

	// Setup
	queue_t q = queue_create();
	int data = 5, temp=0, *store = &temp;
	queue_enqueue(q, &data);
	queue_func_t cb = (queue_func_t) &__test_iterate_func_stop_fxn__;

	// Test
	int retval = queue_iterate(q, cb, NULL, (void**) &store);
	assert(retval == 0);
	assert(store == &data);

	// Teardown
	queue_delete(q, &data);
	queue_destroy(q);
}

void test_iterate() {
	printf("test_iterate\n");

	// Setup
	queue_t q = queue_create();
	int data = 5, *store;
	queue_enqueue(q, &data);
	queue_func_t cb = (queue_func_t) &__test_iterate_fxn__;

	// Test
	int retval = queue_iterate(q, cb, NULL, (void**) &store);
	assert(retval == 0);
	assert(data == 6);

	// Teardown
	queue_delete(q, &data);
	queue_destroy(q);

	// Other cases
	test_iterate_q_null();
	test_iterate_func_null();
	test_iterate_func_stop();
	test_iterate_func_stop_data_def();
}

int main(int argc, char* argv[]) {
	test_create();
	test_destroy();
	test_nq();
	test_dq();
	test_length();
	test_delete();
	test_iterate();
	printf("All tests PASSED\n");
	return 0;
}
