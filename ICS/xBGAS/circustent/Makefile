.PHONY: src

CC=${RVCC}
ARCH=rv64imafdc
RV_OBJDUMP=riscv64-unknown-elf-objdump

ISA_SOURCES := $(wildcard *.c)
ISA_HEADERS := $(wildcard *.h)
ISA_EXES=$(ISA_SOURCES:.c=.exe)
RISCV_GCC_OPTS ?= -DPREALLOCATE=1 -mcmodel=medany -static -std=gnu17 -O0 -ffast-math -fno-common -fno-builtin-printf -march=$(ARCH) -mabi=lp64d
CFLAGS = -I$(XBGAS_RUNTIME)/include  -I$(REV)/common/syscalls  -I$(REV)/test/include

ifeq "$(RVCC)" "riscv64-unknown-elf-gcc"
	RISCV_GCC_OPTS += -fno-tree-loop-distribute-patterns
endif

all:$(ISA_EXES)
$(ISA_TESTS): $(ISA_EXES)
%.exe:%.c $(ISA_HEADERS)
#	@echo "RISCV_GCC_OPTS = $(RISCV_GCC_OPTS)"
	$(CC) $(CFLAGS) $(RISCV_GCC_OPTS)  -o $@ $< -L$(XBGAS_RUNTIME)/lib -lxbrtime -lm

dump: $(ISA_EXES)
	$(foreach var,$(ISA_EXES),$(RV_OBJDUMP) -d $(var) > $(var).dump;)

clean:
	rm -Rf *.exe *.dump

#-- EOF