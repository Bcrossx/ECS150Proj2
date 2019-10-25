# Target library
lib := libuthread.a
# objs := queue.o uthread.o preempt.o context.o
objs := queue.o

all: $(lib)

libuthread.a: $(objs)
	ar rcs $(lib) $(objs)

queue.o: queue.c
	cc -c -g -Wall -Werror queue.c

# uthread.o: uthread.c
# 	cc -c -g -Wall -Werror uthread.c
#
# preempt.o: preempt.c
# 	cc -c -g -Wall -Werror preempt.c
#
# context.o:
# 	cc -c -g -Wall -Werror context.c

clean:
	rm -f $(lib)
