# Compiler and flags
CC = shmemcc
CFLAGS = -O3 -Wall

# Target executable
TARGET = oshm_put_mr.exe

# Source files
SRC = oshm_put_mr.c

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

# Clean up build files
clean:
	rm -f $(TARGET)

# Run the benchmark (example with 1024 bytes)
run: $(TARGET)
	oshrun -np 2 ./$(TARGET) 1024

# Run with a user-defined message size. Example: make run-msg-size SIZE=1024
run-msg-size: $(TARGET)
	@if [ -z "$(SIZE)" ]; then \
	  echo "Usage: make run-msg-size SIZE=<message_size_in_bytes>"; \
	else \
	  oshrun -np 2 ./$(TARGET) $(SIZE); \
	fi