#
# Makefile
#
# makefile: ex1
#
# Copyright (C) 2017-2025 Tactical Computing Laboratories, LLC
# All Rights Reserved
# contact@tactcomplabs.com
#
# See LICENSE in the top level directory for licensing details
#

.PHONY: src

CC=${RVCC}
ARCH=rv64imafdc

ISA_SOURCES := $(wildcard *.c)
ISA_HEADERS := $(wildcard *.h)
ISA_EXES=$(ISA_SOURCES:.c=.exe)
RISCV_GCC_OPTS ?=  -mcmodel=medany -static -std=gnu17 -O0 -ffast-math -fno-common -fno-builtin-printf -march=$(ARCH) -mabi=lp64d
CFLAGS = -I$(XBGAS_RUNTIME)/include  -I$(REV)/common/syscalls 

ifeq "$(RVCC)" "riscv64-unknown-elf-gcc"
	RISCV_GCC_OPTS += -fno-tree-loop-distribute-patterns
endif

all:$(ISA_EXES)
$(ISA_TESTS): $(ISA_EXES)
%.exe:%.c $(ISA_HEADERS)
#	@echo "RISCV_GCC_OPTS = $(RISCV_GCC_OPTS)"
	$(CC) $(CFLAGS) $(RISCV_GCC_OPTS)  -o $@ $< -L$(XBGAS_RUNTIME)/lib -lxbrtime -lm
clean:
	rm -Rf *.exe

#-- EOF