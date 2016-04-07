DIRS   = src apps
DOCDIR = $(DESTDIR)/usr/share/doc/libc3db
MANDIR = $(DESTDIR)/usr/share/man


.PHONY: clean debug test

all:
	@mkdir -p bin lib
	@for d in $(DIRS); do ( cd $$d && $(MAKE) $(MFLAGS) all ); done

debug:
	@mkdir -p bin lib
	@for d in $(DIRS); do ( cd $$d && $(MAKE) $(MFLAGS) debug ); done

fast:
	@mkdir -p bin lib
	@for d in $(DIRS); do ( cd $$d && $(MAKE) $(MFLAGS) fast ); done

clean:
	@for d in $(DIRS); do ( echo "Cleaning up in $$d" && cd $$d && $(MAKE) $(MFLAGS) clean ); done
	@rm -f tests/data/*

install:
	@for d in $(DIRS); do ( cd $$d && $(MAKE) $(MFLAGS) install ); done
	@mkdir -p $(DOCDIR)
	@install -m 644 dist/db_format.txt $(DOCDIR)/db_format.txt
	@mkdir -p $(MANDIR)/man3
	@gzip -c dist/libc3db.3 > $(MANDIR)/man3/libc3db.3.gz

uninstall:
	@for d in $(DIRS); do ( cd $$d && $(MAKE) $(MFLAGS) uninstall ); done
	@rm -rf $(DOCDIR)
	@rm -f $(MANDIR)/man3/libc3db.3.gz

test:
	@( cd tests && ./run.sh )

testclean:
	@rm -f tests/data/* tests/gmon.out

