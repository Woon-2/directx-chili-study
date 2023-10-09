#include "GenOneHotEncode.hpp"

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

CONSTEXPR auto APP_NAME = "GenOneHotEncode.exe"sv;

void help(std::ostream& os)
{
    os << "This is a macro generator which outputs sort of macros called ONEHOT_ENCODE.\n"
        << "ONEHOT_ENCODE assigns every single argument with different bit, ascending from 0th bit, which is 0x01.\n"
        << "ONEHOT_ENCODE is intended to use with identifiers (especially with enum) so that it becomes able to assign the values in preprocessing.\n"
        << "Usage: " << APP_NAME << '\n'
        << "\t-n=<max_arguments>    # ONEHOT_ENCODE with n arguments needs helper macros of n, so it may hurt performance if n is too large.\n"
        << "\t[-s|--suffix=<literal_suffix>]     # The values to be assigned can be different types from int, e.g. char for memory saving, unsinged long long for range extending, so specify the type suffix if you are looking for other type rather than int.\n"
        << "\t[-o|--out|--output=<output_path>]    # specify output path if you desire file output.\n";
}

int main(int argc, const char* const argv[])
{
    auto parser = gclp::parser(
        APP_NAME,
        gclp::required<std::size_t>(
            {'n'}, {"number"}, "maximum iteration of ITERATE_CALL"sv
        ),
        gclp::optional<std::string>(
            {'s'}, {"suffix"}, "suffix of the values going to be assigned"sv
        )->defval(""),
        gclp::optional<std::filesystem::path>(
            {'o'}, {"out", "output"}, "output file path of the result"sv
        ),
        gclp::optional<bool>(
            {'h'}, {"help"}, "print help message"sv
        )->defval(false)
    );

    auto [n, s, o, h] = parser.parse(argc, argv);
    if (parser.error() || h) {
        if (!h) {
            std::cerr << parser.error_message();
        }
        help(std::cerr);
        return 0;
    }

    if ( !o.empty() ) {
        std::ofstream out(o);
        genOneHotEncodeMacros(out, n, s);
    }
    else {
        genOneHotEncodeMacros(std::cout, n, s);
    }
}