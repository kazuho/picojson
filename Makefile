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
	rm -fr build
	rm -f test-core test-core-int64

install:
	mkdir -p build
	cmake -DCMAKE_INSTALL_PREFIX=$(prefix) -DCMAKE_INSTALL_INCLUDEDIR=$(includedir) -S . -B build
	cmake --build build --target install

uninstall:
	rm -f $(DESTDIR)$(includedir)/picojson.h
	rm -f $(DESTDIR)$(libdir)/pkgconfig/picojson.pc
	rm -f $(DESTDIR)$(libdir)/cmake/picojson/picojson-config.cmake
	rm -f $(DESTDIR)$(libdir)/cmake/picojson/picojson-config-version.cmake
	rm -f $(DESTDIR)$(libdir)/cmake/picojson/picojson-targets.cmake

clang-format: picojson.h examples/github-issues.cc examples/iostream.cc examples/streaming.cc
	clang-format -i $?

.PHONY: test check clean install uninstall clang-format
