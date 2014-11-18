#include "../picojson.h"
#include "gmp-traits.h"

using namespace std;
using namespace picojson;

using namespace std;
  
static bool success = true;
static int test_num = 0;

static void ok(bool b, const char* name = "")
{
  if (! b)
    success = false;
  printf("%s %d - %s\n", b ? "ok" : "ng", ++test_num, name);
}

static void done_testing()
{
  printf("1..%d\n", test_num);
}

template <typename T> void is(const T& x, const T& y, const char* name = "")
{
  if (x == y) {
    ok(true, name);
  } else {
    ok(false, name);
  }
}

int main(void)
{
    typedef picojson::value_t<large_int_type_traits> gmp_value;
    
#if PICOJSON_USE_LOCALE
  setlocale(LC_ALL, "");
#endif
  
  // constructors
#define TEST(expr, expected) \
    is(gmp_value expr .serialize(), string(expected), "picojson::value" #expr)
  
#define hugestr "1234567890123456789012345678901234567890"
  mpz_class hugeint(hugestr);
  TEST( (true),  "true");
  TEST( (false), "false");
  TEST( (new gmp_double_pair(42.0)),   "42");
  TEST( (new gmp_double_pair(42)),   "42");
  TEST( (new gmp_double_pair(hugeint)), hugestr);
  TEST( (string("hello")), "\"hello\"");
  TEST( ("hello"), "\"hello\"");
  TEST( ("hello", 4), "\"hell\"");
    
#undef TEST
  
#define TEST(in, type, cmp, serialize_test) {				\
    gmp_value v;							\
    const char* s = in;							\
    string err = picojson::parse(v, s, s + strlen(s));			\
    ok(err.empty(), in " no error");					\
    ok(v.is<type>(), in " check type");					\
    is<type>(v.get<type>(), cmp, in " correct output");			\
    is(*s, '\0', in " read to eof");					\
    if (serialize_test) {						\
      is(v.serialize(), string(in), in " serialize");			\
    }									\
  }
  TEST("false", bool, false, true);
  TEST("true", bool, true, true);
  TEST("90.5", gmp_double_pair, gmp_double_pair(90.5), false);
  TEST("90", gmp_double_pair, gmp_double_pair(mpz_class("90")), false);
  TEST(hugestr, gmp_double_pair, gmp_double_pair(hugeint), false);
#undef TEST

  done_testing();

  return success ? 0 : 1;
}
