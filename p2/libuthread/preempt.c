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
sigset_t mask, origMask;

void preempt_disable(void)
{
	sigprocmask(SIG_SETMASK, &origMask, NULL); // revert mask, unblock sigvtalrm
}

void preempt_enable(void)
{
	sigemptyset(&mask);
	sigaddset(&mask, SIGVTALRM);
	sigprocmask(SIG_BLOCK, &mask, &origMask);
}

void preempt_start(void)
{
	// init sig handler
	catch_sigvalrm();

	// Add timer
	struct itimerval timer;
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 10000;
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 10000;
	setitimer(ITIMER_VIRTUAL, &timer, NULL);
}

void sig_valrm_handler(int signum, siginfo_t *info, void *ptr)
{
	preempt_disable();
	printf("s");
	uthread_yield();
}

void catch_sigvalrm()
{
	static struct sigaction sigact;

	memset(&sigact, 0, sizeof(sigact));
	sigact.sa_sigaction = sig_valrm_handler;
	sigact.sa_flags = SA_SIGINFO;

	sigaction(SIGVTALRM, &sigact, NULL);
}
