CC     = /usr/bin/gcc -std=gnu99 -Wall -Wshadow -Wpedantic -Wno-pointer-arith

NAME   = v1

OBJS   = init.o read.o write.o utils.o
OUT    = ../$(NAME).o

IFLAGS = -I.
DFLAGS = -g -pg -ggdb3
CFLAGS = $(IFLAGS) -shared -fPIC -DPIC

all: $(OUT)

debug: CFLAGS += $(DFLAGS)
debug: $(OUT)

fast: CFLAGS += -O2
fast: $(OUT)

$(OUT): $(OBJS)
	ld -r $(OBJS) -o $(OUT)
	@echo "Built C3/trivalent v1 code object."

clean:
	@rm -f $(OBJS) $(OUT)


