/*
 * Examples how to store simple C++ objects
 * in JSON using `picojson`.
 */
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "../picojson.h"
 // Alias `picojson::value` to `json` to save typing
typedef picojson::value json;


int main() {

    // Assume you have these C++ objects
    // and we want to store them in a JSON file.

    std::string s = "s";
    int i = 0;
    double d = 42;

    std::vector<int> vi;
    vi.push_back(42);
    vi.push_back(43);

    std::vector<std::string> vs;
    vs.push_back("a");
    vs.push_back("bb");

    std::map<std::string, int> msi;
    msi["a"] = 0;
    msi["qwer"] = 1;

    // Assume you want to store them in a JSON object,
    // i.e. a `picojson::value::object` called `data`.

    json::object data;
    data["s"] = json(s);
    // Note: `json(int)` doesn't work, you have to case to double!?
    data["i"] = json(double(i));
    data["d"] = json(d);

    // Note: there's no helper function to convert `std::vector<T>`
    // to `picojson::value::array`, which is a `std::vector<picojson::value>`.
    {
        json::array temp;
        for (auto item : vi) {
            temp.push_back(json(double(item)));
        }
        data["vi"] = json(temp);
    }

    {
        json::array temp;
        for (auto item : vs) {
            temp.push_back(json(item));
        }
        data["vs"] = json(temp);
    }

    {
        json::object temp;
        for (auto item : msi) {
            temp[item.first] = json(double(item.second));
        }
        data["msi"] = json(temp);
    }

    // Now it's easy to serialize `data` to a JSON string
    // that can be printed or written to file.

    std::string json_string = json(data).serialize(true);

    std::cout << json_string << std::endl;

    std::ofstream fh("data.json");
    fh << json_string << std::endl;
    fh.close();

    return 0;

}
