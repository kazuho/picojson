#include <string>
#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <gmpxx.h>
#include <sstream>

class gmp_double_pair
{
    mpz_class large_int;
    double value;
    bool use_double;
public:
    gmp_double_pair()
    : value(0), use_double(true)
    { }
    
    explicit gmp_double_pair(double d)
    : value(d), use_double(true)
    { }    
    
    explicit gmp_double_pair(const mpz_class& bigint)
    : large_int(bigint), value(0), use_double(false)
    { }
    
    bool evaluate_as_boolean() const
    { 
        if(use_double)
            return value == 0;
        else
            return large_int == 0;
    }
    
    static std::pair<gmp_double_pair*, bool> from_str_double(const std::string& s)
    {
        char* endp;
        double f = strtod(s.c_str(), &endp);
        if(endp == s.c_str() + s.size()) {
            return std::make_pair(new gmp_double_pair(f), true);
        }
        return std::make_pair(new gmp_double_pair(0.0), false);
    }
    static std::pair<gmp_double_pair*, bool> from_str(const std::string& s)
    {
        if(s.find(".") != std::string::npos)
        {
            return from_str_double(s);
        }
        else
        {
            try
            {
                // The following code is just designed to turn any valid
                // json integer into a gmp number.
                int loc = s.find_first_of("eE");
                if(loc == std::string::npos)
                {
                    return std::make_pair(new gmp_double_pair(mpz_class(s)), true);
                }
                
                std::string arg1(s.begin(), s.begin() + loc);
                std::string arg2(s.begin() + loc + 1, s.end());

                std::cerr << arg1 << ":" << arg2 << ":\n";
                
                if(arg1.empty() || arg2.empty())
                    throw std::invalid_argument("?");
                
                char* endp;
                long longexp = strtol(s.c_str() + loc + 1, &endp, 10);
                if(endp != s.c_str() + s.size())
                    throw std::invalid_argument("?");
                
                if(longexp < 0)
                {
                    // Back to a double!
                    return from_str_double(s);
                }
                    
                mpz_class exponent;
                mpz_ui_pow_ui(exponent.get_mpz_t(), 10, longexp);
                mpz_class result = mpz_class(arg1) * exponent;
                return std::make_pair(new gmp_double_pair(result), true);
            }
            catch(std::invalid_argument)
            { return std::make_pair(new gmp_double_pair(0.0), false); }
        }
    }
    
    std::string to_str() const
    {
        if(use_double)
        {
            char buf[256];
            double tmp;
            snprintf(buf, sizeof(buf), fabs(value) < (1ULL << 53) && modf(value, &tmp) == 0 ? "%.f" : "%.17g", value);
            return buf;
        }
        else
        {
            std::ostringstream oss;
            oss << large_int;
            return oss.str();
        }   
    }
    
    friend std::ostream& operator<<(std::ostream& o, const gmp_double_pair& s)
    {
        if(s.use_double)
            return o << s.value;
        else
            return o << s.large_int;
    }
    
    friend bool operator==(const gmp_double_pair& lhs, const gmp_double_pair& rhs)
    {
        if(lhs.use_double != rhs.use_double)
            return false;
        if(lhs.use_double)
            return lhs.value == rhs.value;
        else
            return lhs.large_int == rhs.large_int;
    }
};


template<typename T>
struct wrap_number_traits {
  typedef T* value_type;
  typedef T return_type;
  static return_type& to_return_type(value_type& t){
    return *t;
  }
  static value_type default_value() { return new T(); }
  static void construct(value_type &slot, value_type n) {
    slot = new T(*n);
  }
  
  static void destruct(value_type &slot) {delete slot; }
  static bool evaluate_as_boolean(value_type n) {
    return n->evaluate_as_boolean();
  }
  static std::pair<value_type, bool> from_str(const std::string& s) {
      return T::from_str(s);
  }

  static std::string to_str(value_type n) {
      return n->to_str();
  }
};

struct large_int_type_traits {
  typedef wrap_number_traits<gmp_double_pair> number_traits;
};