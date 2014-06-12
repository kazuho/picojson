test:
	$(CXX) -Wall -DTEST_PICOJSON -x c++ - < picojson.h && ./a.out

.PHONY: test
