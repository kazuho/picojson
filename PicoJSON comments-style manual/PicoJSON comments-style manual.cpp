#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#include <string>
#include "../../picojson.h"

using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
    //PicoJSON is a minimal-size json serialization/deserialization library.
    //This manual is as minimal as PicoJSON is.

    //Let's begin with preparring some variables that we'll need further.
    const char *s; //Pointer to the input to the parser.
    string err; //Container for error messages. Zero length of it indicates that no errors happened.
    picojson::value v1; //This is an intermediate container for serialized/deserialized values of any supported type.
    //Supported types include: boolean, numeric, unicode and ascii string, object and array.

    //JSON serialization is very easy task that can be accomplished without any third-party libraries.
    //So that we'll concern desearilization using PicoJSON.
   
    //All you need to deserialize JSON message is to feed it to picojson::parse function.
    s = "[]"; //We'll take empty array as an example of JSON.
    err = picojson::parse(v1, s, s + strlen(s)); //All magic happens inside "parse" function.
    if (err.length() != 0) //If nothing bad happened the string is empty.
        cout << "empty array have not been parsed properly, error message: " << err << endl; 

    //Here I define a macros to hide the code that we have already studied.
    //Although I know that it's not recommended to use macroses in code,
    //I'll adopt this technique to keep the story flow straight.
#define PARSE_VALUE(in, description) { \
    err = picojson::parse(v1, in, in + strlen(in)); \
    if (err.length() != 0) \
        cout << #description " have not been parsed properly, error message: " << err << endl; \
    }

    //Here's how values of supported types can be represented.
    //Boolean types can be expressed as true/false strings not enclosed in quotes.
    s = "\"bool1\": true";
    PARSE_VALUE(s, "boolean values representation");

    //Numeric types can be written in usual and scientific notations.
    //PicoJSON doesn't distinguish between integer and float types.
    //Number must be decimal and the first symbor must be digit.
    s = "[ 1, 1.5, 4, 2.0e+1, 3.1E+10 ]";
    PARSE_VALUE(s, "numeric values representations");

    //Strings are represented as sequences of literals enclosed in double quotes.
    //Literals can be encoded in ASCII or UTF-8 encoding.
    //Unicode symbols outside of string values are illegal and will likely cause the parse function to fail and return error message.
    s = "{ \"ascii_string\": \"some string\", \"unicode_string\": \"\\u0061\\u30af\\u30ea\\u30b9\" }";
    PARSE_VALUE(s, "string values representations");
    //Disclamer: the unicode string for the previous example is taken from the PicoJSON test suite.

    //There are two container types: objects and arrays.
    //Container types can hold within values of any type.
    //Objects are always enclosed in curly braces.
    s = "{}"; //This is a valid object, despite that it's empty.
    //Arrays in turn are enclosed in square brackets.
    s = "[]";

    //Objects' fields always have names.
    s = "{ \"boolean_val\": true, \"integer_val\": 2, \"string_val\": \"megastring\" }";
    PARSE_VALUE(s, "object with fields");

    //Arrays can be on the top level as well and their fields don't have names.
    s = "[ 1, 2.0e+1, \"three\"]";
    PARSE_VALUE(s, "array on the top level");

    //Objects can hold arrays and other objects.
    s = "{ \"array\": [ true, false, 0.1, \"some sort of string\" ], \"the_object\": { \"the_field\": \"another string\" } }";
    PARSE_VALUE(s, "object holding array");

    //Arrays can hold objects and other arrays. Nesting level is not limited.
    s = "{ \"nested_level1_arr\": [ { \"nested_level2_arr\": [ { \"nested_level3_arr\": [] } ] } ] }";
    PARSE_VALUE(s, "nested arrays and objects");


    //But how to get data stored in picojson::value variable?
    //Internally picojson::value represent its data as picojson::object or picojson::array types depending on top-level value.
    //picojson::object is simply std::map specialized as <std::string, picojson::value>
    //and picojson::array is std::vector specialized as <picojson::value>.
    //Let's explore a simple picojson::value object
    s = "{ \"bool1\": true, \"double1\": 10.1, \"int1\": 5, \"string1\": \"some very important string\" }";
    PARSE_VALUE(s, "a simple object containing some fields");

    if (v1.is<picojson::object>()) //Make sure that the top-level value is an object
    {
        picojson::object obj = v1.get<picojson::object>(); //Store reference of the top-level object
        try
        {
            bool b = obj["bool1"].get<bool>(); //Use indexing operator of std::map
            double d = obj.at("double1").get<double>(); //C++11-style "at" function
        }
        catch(std::out_of_range &e)
        {
            //If element not found std::map throws std::out_of_range exception.
        }

        //It's possible to avoid exception handling using iterators.
        picojson::object::iterator it = obj.find("bool1");
        if (it != obj.end())
            bool b = (*it).second.get<bool>();

        //It's also possible to enumerate all map's values using iterators.
        for (picojson::object::iterator it1 = obj.begin(); it1 != obj.end(); it1++)
        {
            if ((*it1).second.is<bool>())
                bool b = (*it1).second.get<bool>();
        }
        
        //But you don't have to mess with STL-type containers because picojson::value presents "contains" checking and "get" access functions.
        if(v1.contains("bool1") && v1.get("bool1").is<bool>()) //Check that the object holds requested field.
            bool bool1 = v1.get("bool1").get<bool>();  //Access the field
    }


    //Let's find out how you can access values stored in array.
    s = "[ true, 3, 2.1, \"the string\"]";
    PARSE_VALUE(s, "array containing values of supported types");
    if (v1.is<picojson::array>()) //Check that top-level value is an array
    {
        picojson::array arr = v1.get<picojson::array>();
        bool b = arr[0].get<bool>();
        int i = arr.at(1).get<double>(); //Note the way the int value was accessed.
        //That's because PicoJSON doesn't distinct between double and int types.
        //We can handle this situation easily using C++'s implicit (or explisit) type convertion.
        //We'll just say that we requesting a double and send it to int variable.
        //If you forget this, your linker will remind you with "unresolved symbol" error.

        //Enumerating all array's elements is similar to emumerating object's elements.
        for (picojson::array::iterator it1 = arr.begin(); it1 != arr.end(); it1++)
        {
            if ((*it1).is<bool>())
                bool b = (*it1).get<bool>();
        }

        //Again you can use picojson::value methods to access its fields.
        for (int i = 0; i < v1.get<picojson::array>().size(); i++)
        {
            if (v1.get(i).is<double>())
                double d = v1.get(i).get<double>();
        }
    }
 
    cout << "If no errors've happenned, this line is the first one. Hit any key to exit.";
    _getch();
    return 0;

    //That's it. Now you khow everything you need to use PicoJSON.
    //Get the latest PicoJSON version from its repository: https://github.com/kazuho/picojson
    //Find more examples in the PicoJSON repository: https://github.com/kazuho/picojson/tree/master/examples
}