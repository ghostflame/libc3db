CC     = /usr/bin/gcc -Wall -Wshadow

MAXV   = 0
MINV   = 1
MICV   = 0

VER    = $(MAXV).$(MINV).$(MICV)

LNM    = libc3db
LIB    = $(LNM).so
RKV    = $(LNM).a
LDIR   = ../lib
LIBT   = $(LDIR)/$(LIB)
RKVT   = $(LDIR)/$(RKV)

IHDR   = ../include/c3db.h
OBJS   = utils.o init.o actions.o
HDRS   = $(IHDR) c3_internal.h

DBVS   = v1
DBVO   = $(DBVS:%=%.o)
DBVT   = all

LIBDIR = /usr/lib
INCDIR = /usr/include

IFLAGS = -I.
DFLAGS = -g -pg -ggdb3
CFLAGS = $(IFLAGS) -fPIC -DPIC
LFLAGS = -shared -fPIC -DPIC -Wl,-soname,$(LIB).$(MAXV)

all: versions $(LIBT) $(RKVT)

debug: CFLAGS += $(DFLAGS)
debug: LFLAGS += $(DFLAGS)
debug: DBVT = debug
debug: versions $(LIBT) $(RKVT)

fast: CFLAGS += -O2
fast: DBVT = fast
fast: versions $(LIBT) $(RKVT)

versions:
	@-for d in $(DBVS); do ( cd $$d; $(MAKE) $(MFLAGS) $(DBVT) ); done

$(LIBT): $(OBJS)
	@mkdir -p $(LDIR)
	$(CC) -o $(LIBT) $(LFLAGS) $(OBJS) $(DBVO)
	@( cd ../lib; ln -s $(LIB) $(LIB).$(MAXV) )
	@echo "Built C3/trivalent database lib."

$(RKVT): $(OBJS)
	@mkdir -p $(LDIR)
	@ar crs $(RKVT) $(OBJS) $(DBVO)
	@ranlib $(RKVT)
	@echo "Built C3/trivalent database archive."

install:
	@install -m644 $(RKVT) $(LIBDIR)/$(RKV)
	@install -m644 $(LIBT) $(LIBDIR)/$(LIB).$(VER)
	@rm -f $(LIBDIR)/$(LIB).$(MAXV) $(LIBDIR)/$(LIB)
	@ln -s $(LIBDIR)/$(LIB).$(VER) $(LIBDIR)/$(LIB).$(MAXV)
	@ln -s $(LIBDIR)/$(LIB).$(VER) $(LIBDIR)/$(LIB)
	@install -m644 $(IHDR) $(INCDIR)/$(IHDR)
	@ldconfig
	@echo "Installed C3/trivalent database libs."

uninstall:
	@rm -f $(LIBDIR)/$(LIB).$(VER) $(LIBDIR)/$(LIB).$(MAXV) $(LIBDIR)/$(LIB)
	@ldconfig
	@echo "Uninstalled C3/trivalent database libs."

clean:
	@-for d in $(DBVS); do ( cd $$d; $(MAKE) clean ); done
	@rm -f core* *.core $(OBJS)
	@( cd ../lib && rm -f $(LIB) $(LIB).$(MAXV) $(RKV) )


