CC = gcc
#CFLAGS = -Wall -Wextra

SRCS = autocomplete.c execute.c history.c logChaining.c main.c raw.c redirect.c tokenize.c signal_handle.c
OBJS = $(SRCS:.c=.o)

TARGET = shell

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(MAKE) all
	rm -f *.o
	