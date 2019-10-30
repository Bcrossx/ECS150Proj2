#include <stdio.h>

#include "uthread.h"
#include "preempt.h"

char* str = "This only prints with preempt\n";

int thread2(void* arg) {
	printf("%s", str);
	return 0;
}

int thread1(void* arg) {
	while(1);
	return 0;
}

void test_preempt() {
	uthread_create(thread1, NULL);
	int id = uthread_create(thread2, NULL);
	uthread_join(id, NULL);
}

int main(int argc, char* argv[]) {
	test_preempt();
	return 0;
}
