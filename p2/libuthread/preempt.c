#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "preempt.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

void catch_sigvalrm();
sigset_t mask;

void preempt_disable(void)
{
	sigemptyset(&mask);
	sigaddset(&mask, SIGVTALRM);
	sigprocmask(SIG_BLOCK, &mask, NULL);
}

void preempt_enable(void)
{
	sigemptyset(&mask);
	sigaddset(&mask, SIGVTALRM);
	sigprocmask(SIG_UNBLOCK, &mask, NULL); // revert mask, unblock sigvtalrm
}

void preempt_start(void)
{
	// init sig handler
	catch_sigvalrm();

	// Add timer
	struct itimerval timer;
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 1000000 / HZ;
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 1000000 / HZ;
	setitimer(ITIMER_VIRTUAL, &timer, NULL);
}

void sig_vtalrm_handler(int signum)
{
	preempt_disable();
	uthread_yield();
}

void catch_sigvalrm()
{
	static struct sigaction sigact;
	memset(&sigact, 0, sizeof(sigact)); // init all zeros
	sigact.sa_handler = sig_vtalrm_handler;
	sigaction(SIGVTALRM, &sigact, NULL);
}
