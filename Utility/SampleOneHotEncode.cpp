#include "OneHotEncode.hpp"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <ranges>
#include <string_view>

#define CASE(s, i) std::cout << s << ": " << i << '\n'

#define TUPLES Math, Physics, Chemistry, Biology, English, Korean, Japanese, History, Economy
#define STR(...) __STR(__VA_ARGS__)
#define __DUMMY
#define __STR(...) __DUMMY ## #__VA_ARGS__

using namespace std::literals::string_view_literals;

enum Class {
    ONEHOT_ENCODE(TUPLES)
};

int main()
{
    std::cout << "ONEHOT_ENCODE(" << STR(TUPLES) << ") constructs\n";
    
    auto tuplesStr = std::views::split( std::string(STR(TUPLES)), ", "sv );
    auto tuplesInt = std::vector<int>{TUPLES};
    
    auto sFirst = tuplesStr.begin();
    auto sLast = tuplesStr.end();
    auto iFirst = tuplesInt.begin();

    while (sFirst != sLast) {
        std::cout << static_cast<std::string>(*sFirst) << ": 0x" << std::hex
            << std::setw(4) << std::setfill('0') << *iFirst << '\n';

        ++sFirst;
        ++iFirst;
    }

    std::cin.get();
}