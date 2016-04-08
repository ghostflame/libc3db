CC     = /usr/bin/gcc -std=gnu99 -Wall -Wshadow -Wpedantic

LDIR   = ../lib
IDIR   = ../include

LNM    = libc3db
LIB    = $(LNM).so
RKV    = $(LNM).a
LIBT   = $(LDIR)/$(LIB)
RKVT   = $(LDIR)/$(RKV)

IHDR   = $(IDIR)/c3db.h
OBJS   = utils.o init.o actions.o
HDRS   = $(IHDR) c3_internal.h

DBVS   = v1
DBVT   = all

IFLAGS = -I.
DFLAGS = -g -pg -ggdb3
CFLAGS = $(IFLAGS) -fPIC -DPIC
LFLAGS = -shared -fPIC -DPIC -Wl,-soname,$(LIB)

all: CFLAGS += -O2
all: DBVT = all
all: versions lib

debug: CFLAGS += $(DFLAGS)
debug: LFLAGS += $(DFLAGS)
debug: DBVT = debug
debug: versions lib

versions:
	@-for d in $(DBVS); do ( cd $$d; $(MAKE) $(MFLAGS) $(DBVT) ); done

lib: $(OBJS)
	@mkdir -p $(LDIR)
	$(CC) -o $(LIBT) $(LFLAGS) $(OBJS) $(DBVS)/*.o
	@echo "Built C3/trivalent database lib."
	@ar crs $(RKVT) $(OBJS) $(DBVS)/*.o
	@ranlib $(RKVT)
	@echo "Built C3/trivalent database archive."

clean:
	@-for d in $(DBVS); do ( cd $$d; $(MAKE) clean ); done
	@rm -f core* *.core $(OBJS)
	@( cd ../lib && rm -f $(LIB) $(RKV) )


