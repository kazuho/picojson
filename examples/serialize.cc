#include <iostream>

#include "../picojson.h"

int main() {

	picojson::value::object root_object;
	root_object["call"] = picojson::value("answer");

	picojson::value root(root_object);

	bool prettify = true;
	std::string serialized = root.serialize(prettify);
	std::cout << serialized;	

	return 0;
}

