prefix=/usr/local
includedir=$(prefix)/include
libdir=$(prefix)/lib

check: test

test: test-core test-core-int64
	./test-core
	./test-core-int64

test-core: picojson.h test.cc picotest/picotest.c picotest/picotest.h
	$(CXX) -Wall test.cc picotest/picotest.c -o $@

test-core-int64: picojson.h test.cc picotest/picotest.c picotest/picotest.h
	$(CXX) -Wall -DPICOJSON_USE_INT64 test.cc picotest/picotest.c -o $@

clean:
	rm -f test-core test-core-int64

install:
	install -d $(DESTDIR)$(includedir)
	install -p -m 0644 picojson.h $(DESTDIR)$(includedir)
	install -d $(DESTDIR)$(libdir)/pkgconfig
	sed -e "s:@prefix@:$(prefix):" \
		-e "s:@includedir@:$(includedir):" \
		picojson.pc.in > $(DESTDIR)$(libdir)/pkgconfig/picojson.pc
	chmod 0644 $(DESTDIR)$(libdir)/pkgconfig/picojson.pc

uninstall:
	rm -f $(DESTDIR)$(includedir)/picojson.h
	rm -f $(DESTDIR)$(libdir)/pkgconfig/picojson.pc

clang-format: picojson.h examples/github-issues.cc examples/iostream.cc examples/streaming.cc
	clang-format -i $?

.PHONY: test check clean install uninstall clang-format
