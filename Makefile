prefix=/usr/local
includedir=$(prefix)/include

test:
	$(MAKE) test-core
	$(MAKE) test-core TEST_OPTS=-DPICOJSON_USE_INT64

test-core:
	$(CXX) -o test-core -Wall $(TEST_OPTS) test.cc picotest/picotest.c && ./test-core

install:
	install -d $(DESTDIR)$(includedir)
	install -p -m 0644 picojson.h $(DESTDIR)$(includedir)

uninstall:
	rm -f $(DESTDIR)$(includedir)/picojson.h

.PHONY: test test-core install uninstall
