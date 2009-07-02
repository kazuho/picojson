/* Copyright 2009 Cybozu Labs, Inc.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY CYBOZU LABS, INC. ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL CYBOZU LABS, INC. OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of Cybozu Labs, Inc.
 *
 */
#ifndef picojson_h
#define picojson_h

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

namespace picojson {
  
  enum {
    undefined_type,
    null_type,
    boolean_type,
    number_type,
    string_type,
    array_type,
    object_type
  };
  
  struct undefined {};
  
  struct null {};
  
  class value {
  public:
    typedef std::vector<value> array;
    typedef std::map<std::string, value> object;
  protected:
    int type_;
    union {
      bool boolean_;
      double number_;
      std::string* string_;
      array* array_;
      object* object_;
    };
  public:
    value(int type = undefined_type);
    ~value();
    value(const value& x);
    value& operator=(const value& x);
    template <typename T> bool is() const;
    template <typename T> const T& get() const;
    template <typename T> T& get();
    operator bool() const;
    const value& get(size_t idx) const;
    const value& get(const std::string& key) const;
    std::string to_str() const;
  };
  
  typedef value::array array;
  typedef value::object object;
  
  inline value::value(int type) : type_(type) {
    switch (type) {
#define INIT(p, v) case p##type: p = v; break
      INIT(boolean_, false);
      INIT(number_, 0.0);
      INIT(string_, new std::string());
      INIT(array_, new array());
      INIT(object_, new object());
#undef INIT
    default: break;
    }
  }
  
  inline value::~value() {
    switch (type_) {
#define DEINIT(p) case p##type: delete p; break
      DEINIT(string_);
      DEINIT(array_);
      DEINIT(object_);
#undef DEINIT
    default: break;
    }
  }
  
  inline value::value(const value& x) : type_(x.type_) {
    switch (type_) {
#define INIT(p, v) case p##type: p = v; break
      INIT(boolean_, x.boolean_);
      INIT(number_, x.number_);
      INIT(string_, new std::string(*x.string_));
      INIT(array_, new array(*x.array_));
      INIT(object_, new object(*x.object_));
#undef INIT
    default: break;
    }
  }
  
  inline value& value::operator=(const value& x) {
    if (this != &x) {
      this->~value();
      new (this) value(x);
    }
    return *this;
  }
  
#define IS(ctype, jtype)			     \
  template <> inline bool value::is<ctype>() const { \
    return type_ == jtype##_type;		     \
  }
  IS(undefined, undefined)
  IS(null, null)
  IS(bool, boolean)
  IS(int, number)
  IS(double, number)
  IS(std::string, string)
  IS(array, array)
  IS(object, object)
#undef IS
  
#define GET(ctype, var)					      \
  template <> inline const ctype& value::get<ctype>() const { \
    return var;						      \
  }							      \
  template <> inline ctype& value::get<ctype>() {	      \
    return var;						      \
  }
  GET(bool, boolean_)
  GET(double, number_)
  GET(std::string, *string_)
  GET(array, *array_)
  GET(object, *object_)
#undef GET
  
  inline value::operator bool() const {
    switch (type_) {
    case undefined_type:
    case null_type:
      return false;
    case boolean_type:
      return boolean_;
    case number_type:
      return number_;
    case string_type:
      return ! string_->empty();
    default:
      return true;
    }
  }
  
  inline const value& value::get(size_t idx) const {
    static value s_undefined(undefined_type);
    assert(is<array>());
    return idx < array_->size() ? (*array_)[idx] : s_undefined;
  }

  inline const value& value::get(const std::string& key) const {
    static value s_undefined(undefined_type);
    assert(is<object>());
    object::const_iterator i = object_->find(key);
    return i != object_->end() ? i->second : s_undefined;
  }
  
  inline std::string value::to_str() const {
    switch (type_) {
    case undefined_type: return "undefined";
    case null_type:      return "null";
    case boolean_type:   return boolean_ ? "true" : "false";
    case number_type:    {
      char buf[256];
#ifdef _MVC_VER
      _snprintf_s(buf, sizeof(buf), "%f", number_);
#else
      snprintf(buf, sizeof(buf), "%f", number_);
#endif
      return buf;
    }
    case string_type:    return *string_;
    case array_type:     return "array";
    case object_type:    return "object";
    default:             assert(0);
#ifdef _MVC_VER
      __assume(0);
#endif
    }
  }
  
  template <typename Iter> class input {
  protected:
    Iter cur_, end_;
    int last_ch_;
    bool ungot_;
    int line_;
  public:
    input(const Iter& first, const Iter& last) : cur_(first), end_(last), last_ch_(-1), ungot_(false), line_(1) {}
    bool eof() const { return cur_ == end_ && ! ungot_; }
    int getc() {
      if (ungot_) {
	ungot_ = false;
	return last_ch_;
      }
      if (cur_ == end_) {
	return -1;
      }
      if (last_ch_ == '\n') {
	line_++;
      }
      last_ch_ = *cur_++ & 0xff;
      return last_ch_;
    }
    void ungetc() {
      if (last_ch_ != -1) {
	assert(! ungot_);
	ungot_ = true;
      }
    }
    Iter cur() const { return cur_; }
    int line() const { return line_; }
    void skip_ws() {
      while (! eof()) {
	if (! isspace(getc())) {
	  ungetc();
	  break;
	}
      }
    }
    enum {
      error,
      negative,
      positive
    };
    int match(const std::string& pattern) {
      skip_ws();
      std::string::const_iterator pi(pattern.begin());
      for (; pi != pattern.end(); ++pi) {
	if (eof()) {
	  break;
	} else if (getc() != *pi) {
	  ungetc();
	  break;
	}
      }
      if (pi == pattern.end()) {
	return positive;
      } else if (pi == pattern.begin()) {
	return negative;
      } else {
	return error;
      }
    }
  };
  
  template<typename Iter> static int _parse_quadhex(input<Iter> &in) {
    int uni_ch = 0, hex;
    for (int i = 0; i < 4; i++) {
      if ((hex = in.getc()) == -1) {
	return -1;
      }
      if ('0' <= hex && hex <= '9') {
	hex -= '0';
      } else if ('A' <= hex && hex <= 'F') {
	hex -= 'A' - 0xa;
      } else if ('a' <= hex && hex <= 'f') {
	hex -= 'a' - 0xa;
      } else {
	return -1;
      }
      uni_ch = uni_ch * 16 + hex;
    }
    return uni_ch;
  }
  
  template<typename Iter> static bool _parse_codepoint(std::string& out, input<Iter>& in) {
    int uni_ch;
    if ((uni_ch = _parse_quadhex(in)) == -1) {
      return false;
    }
    if (0xd800 <= uni_ch && uni_ch <= 0xdfff) {
      if (0xdc00 <= uni_ch) {
	// a second 16-bit of a surrogate pair appeared
	return false;
      }
      // first 16-bit of surrogate pair, get the next one
      if (in.getc() != '\\' || in.getc() != 'u') {
	return false;
      }
      int second = _parse_quadhex(in);
      if (! (0xdc00 <= second && second <= 0xdfff)) {
	return false;
      }
      uni_ch = ((uni_ch - 0xd800) << 10) | ((second - 0xdc00) & 0x3ff);
      uni_ch += 0x10000;
    }
    if (uni_ch < 0x80) {
      out.push_back(uni_ch);
    } else {
      if (uni_ch < 0x800) {
	out.push_back(0xc0 | (uni_ch >> 6));
      } else {
	if (uni_ch < 0x10000) {
	  out.push_back(0xe0 | (uni_ch >> 12));
	} else {
	  out.push_back(0xf0 | (uni_ch >> 18));
	  out.push_back(0x80 | ((uni_ch >> 12) & 0x3f));
	}
	out.push_back(0x80 | ((uni_ch >> 6) & 0x3f));
      }
      out.push_back(0x80 | (uni_ch & 0x3f));
    }
    return true;
  }
  
  template<typename Iter> static bool _parse_string(value& out, input<Iter>& in) {
    // gcc 4.1 cannot compile if the below two lines are merged into one :-(
    out = value(string_type);
    std::string& s = out.get<std::string>();
    while (! in.eof()) {
      int ch = in.getc();
      if (ch == '"') {
	return true;
      } else if (ch == '\\') {
	if ((ch = in.getc()) == -1) {
	  return false;
	}
	switch (ch) {
#define MAP(sym, val) case sym: s.push_back(val); break
	  MAP('"', '\"');
	  MAP('\\', '\\');
	  MAP('/', '/');
	  MAP('b', '\b');
	  MAP('f', '\f');
	  MAP('n', '\n');
	  MAP('r', '\r');
	  MAP('t', '\t');
#undef MAP
	case 'u':
	  if (! _parse_codepoint(s, in)) {
	    return false;
	  }
	  break;
	default:
	  return false;
	}
      } else {
	s.push_back(ch);
      }
    }
    return false;
  }
  
  template <typename Iter> static bool _parse_array(value& out, input<Iter>& in) {
    out = value(array_type);
    array& a = out.get<array>();
    if (in.match("]") == input<Iter>::positive) {
      return true;
    }
    do {
      a.push_back(value());
      if (! _parse(a.back(), in)) {
	return false;
      }
    } while (in.match(",") == input<Iter>::positive);
    return in.match("]") == input<Iter>::positive;
  }
  
  template <typename Iter> static bool _parse_object(value& out, input<Iter>& in) {
    out = value(object_type);
    object& o = out.get<object>();
    if (in.match("}") == input<Iter>::positive) {
      return true;
    }
    do {
      value key, val;
      if (in.match("\"") == input<Iter>::positive
	  && _parse_string(key, in)
	  && in.match(":") == input<Iter>::positive
	  && _parse(val, in)) {
	o[key.to_str()] = val;
      } else {
	return false;
      }
    } while (in.match(",") == input<Iter>::positive);
    return in.match("}") == input<Iter>::positive;
  }
  
  template <typename Iter> static bool _parse_number(value& out, input<Iter>& in) {
    out = value(number_type);
    std::string num_str;
    while (! in.eof()) {
      int ch = in.getc();
      if ('0' <= ch && ch <= '9' || ch == '+' || ch == '-' || ch == '.'
	  || ch == 'e' || ch == 'E') {
	num_str.push_back(ch);
      } else {
	in.ungetc();
	break;
      }
    }
    char* endp;
    out.get<double>() = strtod(num_str.c_str(), &endp);
    return endp == num_str.c_str() + num_str.size();
  }
  
  template <typename Iter> static bool _parse(value& out, input<Iter>& in) {
    int ret = input<Iter>::negative;
#define IS(p)						\
    (ret == input<Iter>::negative			\
     && (ret = in.match(p)) == input<Iter>::positive)
    if (IS("undefined")) {
      out = value(undefined_type);
    } else if (IS("null")) {
      out = value(null_type);
    } else if (IS("false")) {
      out = value(boolean_type);
    } else if (IS("true")) {
      out = value(boolean_type);
      out.get<bool>() = true;
    } else if (IS("\"")) {
      return _parse_string(out, in);
    } else if (IS("[")) {
      return _parse_array(out, in);
    } else if (IS("{")) {
      return _parse_object(out, in);
    } else {
      int ch = in.getc();
      if (ch != -1) {
	in.ungetc();
	if ('0' <= ch && ch <= '9' || ch == '-') {
	  return _parse_number(out, in);
	}
      }
    }
#undef IS
    return ret == input<Iter>::positive;
  }
  
  template <typename Iter> static std::string parse(value& out, Iter& pos, const Iter& last) {
    // setup
    input<Iter> in(pos, last);
    std::string err;
    // do
    if (! _parse(out, in)) {
      char buf[64];
      sprintf(buf, "syntax error at line %d near: ", in.line());
      err = buf;
      while (! in.eof()) {
	int ch = in.getc();
	if (ch == '\n') {
	  break;
	}
	err += ch;
      }
    }
    pos = in.cur();
    return err;
  }
  
  inline static std::string parse(value& out, std::istream& is) {
    std::istreambuf_iterator<char> ii(is.rdbuf());
    return parse(out, ii, std::istreambuf_iterator<char>());
  }
  
}

#endif
#ifdef TEST_PICOJSON

using namespace std;
  
static void plan(int num)
{
  printf("1..%d\n", num);
}

static void ok(bool b, const char* name = "")
{
  static int n = 1;
  printf("%s %d - %s\n", b ? "ok" : "ng", n++, name);
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
  plan(49);
  
  
#define TEST(in, type, cmp) {						\
    picojson::value v;							\
    const char* s = in;							\
    string err = picojson::parse(v, s, s + strlen(s));			\
    ok(err.empty(), in " no error");					\
    ok(v.is<type>(), in " check type");					\
    is(v.get<type>(), cmp, in " correct output");			\
    is(*s, '\0', in " read to eof");					\
  }
  TEST("false", bool, false);
  TEST("true", bool, true);
  TEST("90.5", double, 90.5);
  TEST("\"hello\"", string, string("hello"));
  TEST("\"\\\"\\\\\\/\\b\\f\\n\\r\\t\"", string, string("\"\\/\b\f\n\r\t"));
  TEST("\"\\u0061\\u30af\\u30ea\\u30b9\"", string,
       string("a\xe3\x82\xaf\xe3\x83\xaa\xe3\x82\xb9"));
  TEST("\"\\ud840\\udc0b\"", string, string("\xf0\xa0\x80\x8b"));
#undef TEST

#define TEST(type, expr) {					       \
    picojson::value v;						       \
    const char *s = expr;					       \
    string err = picojson::parse(v, s, s + strlen(s));		       \
    ok(err.empty(), "empty " #type " no error");		       \
    ok(v.is<picojson::type>(), "empty " #type " check type");	       \
    ok(v.get<picojson::type>().empty(), "check " #type " array size"); \
  }
  TEST(array, "[]");
  TEST(object, "{}");
#undef TEST
  
  {
    picojson::value v;
    const char *s = "[1,true,\"hello\"]";
    string err = picojson::parse(v, s, s + strlen(s));
    ok(err.empty(), "array no error");
    ok(v.is<picojson::array>(), "array check type");
    is(v.get<picojson::array>().size(), size_t(3), "check array size");
    ok(v.get(0).is<double>(), "check array[0] type");
    is(v.get(0).get<double>(), 1.0, "check array[0] value");
    ok(v.get(1).is<bool>(), "check array[1] type");
    ok(v.get(1).get<bool>(), "check array[1] value");
    ok(v.get(2).is<string>(), "check array[2] type");
    is(v.get(2).get<string>(), string("hello"), "check array[2] value");
  }
  
  {
    picojson::value v;
    const char *s = "{ \"a\": true }";
    string err = picojson::parse(v, s, s + strlen(s));
    ok(err.empty(), "object no error");
    ok(v.is<picojson::object>(), "object check type");
    is(v.get<picojson::object>().size(), size_t(1), "check object size");
    ok(v.get("a").is<bool>(), "check property exists");
    is(v.get("a").get<bool>(), true,
       "check property value");
  }
  
  {
    picojson::value v;
    const char *s = "falsoooo";
    string err = picojson::parse(v, s, s + strlen(s));
    is(err, string("syntax error at line 1 near: oooo"), "error message");
  }
  
  return 0;
}

#endif
