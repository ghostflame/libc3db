DIRS   = src apps
TARGET = all

VERS = $(shell sed -rn 's/Version:\t(.*)/\1/p' libc3db.spec)
VMAJ = $(shell sed -rn 's/Version:\t([0-9]+)\..*/\1/p' libc3db.spec)


# prefer environment
BINDIR ?= $(DESTDIR)/usr/bin
INCDIR ?= $(DESTDIR)/usr/include
LIBDIR ?= $(DESTDIR)/usr/lib
MANDIR ?= $(DESTDIR)/usr/share/man


.PHONY: clean debug test

all:
	@mkdir -p bin lib
	@for d in $(DIRS); do ( cd $$d && $(MAKE) $(MFLAGS) all ); done

debug:
	@mkdir -p bin lib
	@for d in $(DIRS); do ( cd $$d && $(MAKE) $(MFLAGS) debug ); done

clean:
	@for d in $(DIRS); do ( echo "Cleaning up in $$d" && cd $$d && $(MAKE) $(MFLAGS) clean ); done
	@rm -f tests/data/*

install:
	@echo "Making installation directories"
	@mkdir -p $(BINDIR) $(LIBDIR) $(MANDIR)/man3 $(INCDIR) $(DOCDIR)
	@for d in $(DIRS); do ( cd $$d && $(MAKE) $(MFLAGS) all ); done
	@install -m755 bin/c3db_* $(BINDIR)
	@install -m644 include/c3db.h $(INCDIR)
	@install -m755 lib/libc3db.so $(LIBDIR)/libc3db.so.$(VERSION)
	@install -m755 lib/libc3db.a $(LIBDIR)
	@ln -s $(LIBDIR)/libc3db.so.$(VERSION) $(LIBDIR)/libc3db.so
	@ln -s $(LIBDIR)/libc3db.so.$(VERSION) $(LIBDIR)/libc3db.so.$(VMAJ)
	@gzip -c dist/libc3db.3 > $(MANDIR)/man3/libc3db.3.gz
	@install -m644 LICENSE BUGS README.md dist/db_format.txt $(DOCDIR)
	@ldconfig

uninstall:
	@echo "Warning: this may conflict with an RPM install!"
	@echo "Use make remove to actually remove things."

remove:
	@echo "Todo."

test:
	@( cd tests && ./run.sh )

testclean:
	@rm -f tests/data/*.txt tests/data/*.c3db tests/gmon.out

