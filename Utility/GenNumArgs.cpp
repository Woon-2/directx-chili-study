#include "GenNumArgs.hpp"

#include <iostream>
#include <filesystem>
#include <fstream>

#include "gclp.hpp"

#if defined __cpp_constexpr || (__cplusplus >= 201103L)
#define CONSTEXPR constexpr
#else
#define CONSTEXPR 
#endif

using namespace std::literals;

CONSTEXPR auto APP_NAME = "GenNumArgs.exe"sv;

void help(std::ostream& os) {
    os << "This is a macro generator which outputs sort of macros called NUMARGS.\n"
        << "NUMARGS counts the number of given varadic arguments.\n"
        << "NUMARGS is intended to use with macros that so that it becomes able to use the count in preprocessing.\n"
        << "Usage: " << APP_NAME << '\n'
        << "\t-n|--number=<max_count>\n"
        << "\t[-o|--out|--output=<output_path>]    # specify output path if you desire file output.\n";
}

int main(int argc, const char* const argv[])
{
    auto parser = gclp::parser(
        APP_NAME,
        gclp::required<std::size_t>(
            {'n'}, {"number"}, "maximum count of NUMARGS"sv
        ),
        gclp::optional<std::filesystem::path>(
            {'o'}, {"out", "output"}, "output file path of the result"sv
        ),
        gclp::optional<bool>(
            {'h'}, {"help"}, "print help message"sv
        )->defval(false)
    );

    auto [n, o, h] = parser.parse(argc, argv);
    if (parser.error() || h) {
        if (!h) {
            std::cerr << parser.error_message();
        }
        help(std::cerr);
        return 0;
    }

    if ( !o.empty() ) {
        std::ofstream out(o);
        genNumArgsMacros(out, n);
    }
    else {
        genNumArgsMacros(std::cout, n);
    }
}