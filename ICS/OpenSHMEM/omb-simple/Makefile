# Compiler and flags
CC = shmemcc
CFLAGS = -O0 -Wall

# Source files
SRCS = $(wildcard *.c)
TARGETS = $(SRCS:.c=.exe)

# Default target
all: $(TARGETS)

# Build target
$(TARGETS): %.exe: %.c
	$(CC) $(CFLAGS) -o $@ $< -L$(UCX_HOME)/lib -I$(UCX_HOME)/include -lucs

# Clean up build files
clean:
	rm -f $(TARGETS)