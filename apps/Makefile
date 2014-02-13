CC      = /usr/bin/gcc -Wall -Wshadow

BINDIR  = ../bin
LIBDIR  = ../lib
INCDIR  = ../include
INSDIR  = /usr/bin

DFLAGS  = -g -pg -ggdb3
CFLAGS  = -I$(INCDIR)
RKV     = $(LIBDIR)/libc3db.a

NAMES   = create dump update query
APPS    = $(NAMES:%=c3db_%)


all:
	@-for d in $(APPS); do ( $(CC) $(CFLAGS) -o $(BINDIR)/$$d $$d.c $(RKV) && echo "Built $$d" ); done

debug:
	@-for d in $(APPS); do ( $(CC) $(DFLAGS) $(CFLAGS) -o $(BINDIR)/$$d $$d.c $(RKV) && echo "Built $$d (debug)" ); done

clean:
	@-for d in $(APPS); do rm -f $(BINDIR)/$$d; done

install:
	@-for d in $(APPS) do ( install -m755 $(BINDIR)/$$d $(INSDIR)/$$d && echo "Installed $$d into $(INSDIR)" ); done

uninstall:
	@-for d in $(APPS) do ( rm -f $(INSDIR)/$$d && echo "Removed installed copy of $$d" ); done
