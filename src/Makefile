CC     = /usr/bin/gcc -std=gnu99 -Wall -Wshadow -Wpedantic

LNM    = libc3db
LIB    = $(LNM).so
LIBV   = $(LIB).$(shell sed -rn 's/^Version:\t(.*)/\1/p' ../libc3db.spec)
RKV    = $(LNM).a
LDIR   = ../lib
LIBT   = $(LDIR)/$(LIBV)
RKVT   = $(LDIR)/$(RKV)

IHDR   = ../include/c3db.h
OBJS   = utils.o init.o actions.o
HDRS   = $(IHDR) c3_internal.h

DBVS   = v1
DBVO   = $(DBVS:%=%.o)
DBVT   = all

LIBDIR = $(DESTDIR)/usr/lib
INCDIR = $(DESTDIR)/usr/include

IFLAGS = -I.
DFLAGS = -g -pg -ggdb3
CFLAGS = $(IFLAGS) -fPIC -DPIC
LFLAGS = -shared -fPIC -DPIC -Wl,-soname,$(LIB)

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
	@( cd ../lib; ln -s $(LIBV) $(LIB) )
	@echo "Built C3/trivalent database lib."

$(RKVT): $(OBJS)
	@mkdir -p $(LDIR)
	@ar crs $(RKVT) $(OBJS) $(DBVO)
	@ranlib $(RKVT)
	@echo "Built C3/trivalent database archive."

install:
	@install -m644 $(RKVT) $(LIBDIR)/$(RKV)
	@install -m644 $(LIBT) $(LIBDIR)/$(LIBV)
	@rm -f $(LIBDIR)/$(LIB)
	@ln -s $(LIBDIR)/$(LIBV) $(LIBDIR)/$(LIB)
	@install -m644 $(IHDR) $(INCDIR)/$(IHDR)
	@ldconfig
	@echo "Installed C3/trivalent database libs."

uninstall:
	@rm -f $(LIBDIR)/$(LIB) $(LIBDIR)/$(LIBV) $(LIBDIR)/$(RKV) $(INCDIR)/$(IHDR)
	@ldconfig
	@echo "Uninstalled C3/trivalent database libs."

clean:
	@-for d in $(DBVS); do ( cd $$d; $(MAKE) clean ); done
	@rm -f core* *.core $(OBJS)
	@( cd ../lib && rm -f $(LIB) $(LIBV) $(RKV) )


