CC = gcc
CFLAGS = -Wall -Wextra 
OBJS = main.o raw.o history.o redirect.o
DEPS = raw_input.h history.h redirect.h

all: shell

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

shell: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o shell

clean:
	rm -f *.o shell
