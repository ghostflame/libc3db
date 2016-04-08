CC      = /usr/bin/gcc -std=gnu99 -Wall -Wshadow -Wpedantic

BINDIR  = ../bin
LIBDIR  = ../lib
INCDIR  = ../include

DFLAGS  = -g -pg -ggdb3
CFLAGS  = -I$(INCDIR)
RKV     = $(LIBDIR)/libc3db.a

NAMES   = create dump update query
APPS    = $(NAMES:%=c3db_%)

all: CFLAGS += -O2
all:
	@mkdir -p $(BINDIR)
	@-for d in $(APPS); do ( $(CC) $(CFLAGS) -o $(BINDIR)/$$d $$d.c $(RKV) && echo "Built $$d" ); done

debug:
	@mkdir -p $(BINDIR)
	@-for d in $(APPS); do ( $(CC) $(DFLAGS) $(CFLAGS) -o $(BINDIR)/$$d $$d.c $(RKV) && echo "Built $$d (debug)" ); done

clean:
	@-for d in $(APPS); do rm -f $(BINDIR)/$$d; done


