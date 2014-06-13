test:
	$(MAKE) test-core
	$(MAKE) test-core TEST_OPTS=-DPICOJSON_USE_INT64

test-core:
	$(CXX) -Wall $(TEST_OPTS) -DTEST_PICOJSON -x c++ - < picojson.h && ./a.out

.PHONY: test test-core
