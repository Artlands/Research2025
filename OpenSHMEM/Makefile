# Compiler and flags
CC = shmemcc
CFLAGS = -O3 -Wall

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

# Run all targets with 1024 bytes (example)
run: $(TARGETS)
	@for target in $(TARGETS); do \
		echo "Running $$target with message size 1024"; \
		oshrun -np 2 $$target 1024; \
	done

# Run with a user-defined message size. Example: make run-msg-size SIZE=1024
run-msg-size: $(TARGETS)
	@if [ -z "$(SIZE)" ]; then \
	  echo "Usage: make run-msg-size SIZE=<message_size_in_bytes>"; \
	else \
	  for target in $(TARGETS); do \
	    echo "Running $$target with message size $(SIZE)"; \
	    oshrun -np 2 $$target $(SIZE); \
	  done; \
	fi