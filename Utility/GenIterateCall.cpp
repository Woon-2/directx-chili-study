#include "GenIterateCall.hpp"

#include <iostream>
#include <string_view>
#include <filesystem>
#include <fstream>

#include "gclp.hpp"

#if defined __cpp_constexpr || (__cplusplus >= 201103L)
#define CONSTEXPR constexpr
#else
#define CONSTEXPR 
#endif

using namespace std::literals;

CONSTEXPR auto APP_NAME = "GenIterateCall.exe"sv;

void help(std::ostream& os)
{
    os << "This is a macro generator which outputs sort of macros called ITERATE_CALL.\n"
        << "ITERATE_CALL behaves like std::for_each, it invokes a passed function or macro with every single argument received.\n"
        << "ITERATE_CALL is intended to use with macros so that it becomes able to iterate some stuffs which must be evaluated in preprocessing.\n"
        << "Usage: " << APP_NAME << '\n'
        << "\t-n|--number=<max_arguments>    # ITERATE_CALL with n arguments needs helper macros of n, so it may hurt performance if n is too large.\n"
        << "\t[-o|--out|--output=<output_path>]    # specify output path if you desire file output.\n";
}

int main(int argc, const char* const argv[])
{
    auto parser = gclp::parser(
        APP_NAME,
        gclp::required<std::size_t>(
            {'n'}, {"number"}, "maximum iteration of ITERATE_CALL"sv
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
        genIterateCallMacros(out, n);
    }
    else {
        genIterateCallMacros(std::cout, n);
    }
}