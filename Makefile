test:
	$(MAKE) test-core
	$(MAKE) test-core TEST_OPTS=-DPICOJSON_USE_INT64

test-core:
	$(CXX) -o test-core -Wall $(TEST_OPTS) test.cc picotest/picotest.c && ./test-core

.PHONY: test test-core
