#include <iostream>
#include <iterator>
#include "../picojson.h"

// this example reads a array of hashes (each item representing a 2D point),
// and prints the x and y values to stdout

namespace {
  
  class root_context : public picojson::deny_parse_context {
  public:
    bool parse_array_start() {
      return true; // only allow array as root
    }
    template <typename Iter> bool parse_array_item(picojson::input<Iter>& in, size_t) {
      picojson::value item;
      // parse the array item
      picojson::default_parse_context ctx(&item);
      if (! picojson::_parse(ctx, in)) {
	return false;
      }
      // assert that the array item is a hash
      if (! item.is<picojson::object>()) {
	return false;
      }
      // print x and y
      std::cout << item.get("x") << ',' << item.get("y").to_str()
		<< std::endl;
      return true;
    }
  };
  
}

int main(void)
{
  root_context ctx;
  std::string err;
  
  picojson::_parse(ctx, std::istream_iterator<char>(std::cin),
		   std::istream_iterator<char>(), &err);
  
  if (! err.empty()) {
    std::cerr << err << std::endl;
    return 1;
  }
  
  return 0;
}
