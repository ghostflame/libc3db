DIRS   = src apps
LIBDIR = /usr/lib

.PHONY: clean debug test

all:
	@for d in $(DIRS); do ( cd $$d && $(MAKE) $(MFLAGS) all ); done

debug:
	@for d in $(DIRS); do ( cd $$d && $(MAKE) $(MFLAGS) debug ); done

fast:
	@for d in $(DIRS); do ( cd $$d && $(MAKE) $(MFLAGS) fast ); done

clean:
	@for d in $(DIRS); do ( echo "Cleaning up in $$d" && cd $$d && $(MAKE) $(MFLAGS) clean ); done
	@rm -f tests/data/*

install:
	@for d in $(DIRS); do ( cd $$d && $(MAKE) $(MFLAGS) install ); done

uninstall:
	@for d in $(DIRS); do ( cd $$d && $(MAKE) $(MFLAGS) uninstall ); done

test:
	@( cd tests && ./run.sh )

testclean:
	@rm -f tests/data/* tests/gmon.out

