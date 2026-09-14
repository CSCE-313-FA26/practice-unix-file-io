# CSCE 313 - UNIX file I/O demos
#
#   make           build the seven lecture demos
#   make 06-two-opens  build just one
#   make demo      run them back to back, with headers
#   make extra     build the three that did not fit the slot (see extra/)
#   make clean     remove binaries and everything the demos wrote
#
# Same flags as practice-1: these programs are small enough that the compiler
# should have nothing to say, so any warning you see is worth reading.

CC      = gcc
CFLAGS  = -std=c11 -g -Wall -Wextra -D_GNU_SOURCE

SRCS    = $(wildcard [0-9]*.c)
PROGS   = $(SRCS:.c=)

# Demos that did not earn a slot in the walk. Built into extra/ by `make extra`.
XSRCS   = $(wildcard extra/[0-9]*.c)
XPROGS  = $(XSRCS:.c=)

all: $(PROGS)

extra: $(XPROGS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

demo: all
	@./run-all.sh

clean:
	rm -f $(PROGS) $(XPROGS)
	rm -f right.txt wrong.txt foobar.txt abab.txt big.txt tiny.txt \
	      seekme.txt redirected.txt copy.txt

list:
	@printf '%s\n' $(PROGS)

.PHONY: all extra demo clean list
