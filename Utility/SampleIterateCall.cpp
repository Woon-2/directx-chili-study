#include "IterateCall.hpp"

#include <iostream>
#include <ranges>
#include <algorithm>
#include <vector>
#include <string>

#define ADD_ONE(x) (x + 1)

#define ELEMS 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
#define STR(...) __STR(__VA_ARGS__)
#define __STR(...) __DUMMY ## #__VA_ARGS__
#define __DUMMY

int main()
{
    auto result = std::vector<std::string>{
        ITERATE_CALL( std::to_string, ITERATE_CALL(ADD_ONE, ELEMS) )
    };

    std::cout << "#define ADD_ONE(x) (x + 1)\n";
    std::cout << "ITERATE_CALL(ADD_ONE, " << STR(ELEMS) << ") constructs\n";

    std::cout << '{';
    std::ranges::for_each( result | std::views::take(result.size() - 1),
        [](const auto& s){ std::cout << s << ", "; } );
    std::cout << result.back() << "}\n";

    std::cin.get();
}