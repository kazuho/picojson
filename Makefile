test: picojson.h
	g++ -x c++ $(OPTIMIZE) -Wall -Wextra -DTEST_PICOJSON picojson.h -o test
	prove -v ./test
