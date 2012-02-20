test: picojson.h
	$(CXX) -x c++ $(OPTIMIZE) -Wall -Wextra -DTEST_PICOJSON picojson.h -o test
	prove -v ./test
	rm test
