#include "../picojson.h"

int main(void)
{
  picojson::value v;
  
  // read json value from stream
  std::cin >> v;
  if (std::cin.fail()) {
    std::cerr << picojson::get_last_error() << std::endl;
    return 1;
  }
  
  // dump json object
  std::cout << "---- dump input ----" << std::endl;
  std::cout << v << std::endl;

  // accessors
  std::cout << "---- analyzing input ----" << std::endl;
  if (v.is<picojson::undefined>()) {
    std::cout << "input is undefined" << std::endl;
  } else if (v.is<picojson::null>()) {
    std::cout << "input is null" << std::endl;
  } else if (v.is<bool>()) {
    std::cout << "input is " << (v.get<bool>() ? "true" : "false") << std::endl;
  } else if (v.is<double>()) {
    std::cout << "input is " << v.get<double>() << std::endl;
  } else if (v.is<std::string>()) {
    std::cout << "input is " << v.get<std::string>() << std::endl;
  } else if (v.is<picojson::array>()) {
    std::cout << "input is an array" << std::endl;
    const picojson::array& a = v.get<picojson::array>();
    for (picojson::array::const_iterator i = a.begin(); i != a.end(); ++i) {
      std::cout << "  " << *i << std::endl;
    }
  } else if (v.is<picojson::object>()) {
    std::cout << "input is an object" << std::endl;
    const picojson::object& o = v.get<picojson::object>();
    for (picojson::object::const_iterator i = o.begin(); i != o.end(); ++i) {
      std::cout << i->first << "  " << i->second << std::endl;
    }
  }
  
  return 0;
}
