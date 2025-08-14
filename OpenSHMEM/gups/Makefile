# Compiler and flags
CC = shmemcc
CFLAGS = -O0 -Wall

# Source files excluding util.c
SRCS = $(filter-out $(UTIL_SRC), $(wildcard *.c))

# Object files for each source
OBJS = $(SRCS:.c=.o)

# Executables for each source
EXES = $(SRCS:.c=.exe)

all:$(EXES)

%.exe: %.o
	$(CC) $(CFLAGS) -o $@ $^

# Rule to compile source files into object files
%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -Rf *.exe *.dump *.o

#-- EOF