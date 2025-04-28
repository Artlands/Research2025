# Compiler and flags
CC = shmemcc
CFLAGS = -O0 -Wall

# Utility source file
UTIL_SRC = osu_util.c
UTIL_OBJ = $(UTIL_SRC:.c=.o)

# Source files excluding util.c
SRCS = $(filter-out $(UTIL_SRC), $(wildcard *.c))

# Object files for each source
OBJS = $(SRCS:.c=.o)

# Executables for each source
EXES = $(SRCS:.c=.exe)

all:$(EXES)

%.exe: %.o $(UTIL_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -L$(UCX_HOME)/lib -I$(UCX_HOME)/include -lucs

# Rule to compile source files into object files
%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Rule to compile util
$(UTIL_OBJ): $(UTIL_SRC)
	$(CC) $(CFLAGS) -c -o $@ $< 

clean:
	rm -Rf *.exe *.dump *.o

#-- EOF