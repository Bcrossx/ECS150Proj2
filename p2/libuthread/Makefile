# Target library
lib := libuthread.a
objs := queue.c

all: $(lib)

libuthread.a: $(objs)
	cc -c -g -Wall -Werror -o $(lib) $(objs)

clean:
	rm -f $(lib)
