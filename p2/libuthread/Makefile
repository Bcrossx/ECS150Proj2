# Target library
lib := libuthread.a
objs := queue.o

all: $(lib)

libuthread.a: $(objs)
	ar rcs $(lib) $(objs)

queue.o: queue.c
	cc -c -g -Wall -Werror queue.c

clean:
	rm -f $(lib)
