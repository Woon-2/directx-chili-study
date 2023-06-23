#include "NumArgs.hpp"

#include <iostream>

#define CASE(c) std::cout << #c << ": " << c << '\n'

int main()
{
    CASE(NUMARGS(1, 2, 3, 4));
    CASE(NUMARGS("abc", "def", "ghi", "jkl", "mno", "pqr"));
    CASE(NUMARGS(0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6));

    std::cin.get();
}