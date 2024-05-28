CC = gcc
CFLAGS = -Wall -Wextra -pthread -g
LDFLAGS = -lm

SRCDIR = src
SRCFILES = $(wildcard $(SRCDIR)/*.c)
OBJFILES = $(SRCFILES:.c=.o)

EXECUTABLE = projeto

VPATH = $(SRCDIR)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJFILES) $(EXECUTABLE)
