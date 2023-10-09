#ifndef __GCLP
#define __GCLP

#include <string>
#include <string_view>
#include <optional>
#include <tuple>
#include <ranges>
#include <algorithm>
#include <utility>
#include <iterator>
#include <functional>
#include <type_traits>
#include <concepts>
#include <sstream>
#include <vector>
#include <initializer_list>
#include <limits>
#include <cassert>
#include <format>
#include <bitset>
#include <ios>

/**
 * @brief Macro for creating string literals with correct prefix based on character type.
 *
 * This macro enables the creation of string literals with the appropriate prefix based on the character type.
 * For example, if the character type is 'char', the macro returns the input string. If the character type is 'wchar_t',
 * it returns the input string with the 'L' prefix, and so on for other character types.
 *
 * @param type The character type for which the string literal is created.
 * @param s The input string for which the literal is generated.
 * @return The string literal with the correct prefix based on the character type.
 */
#ifdef __cpp_char8_t
#define __LITERAL(type, s)  \
[]() {  \
    if constexpr ( std::is_same_v<type, char> ) {   \
        return s;   \
    }   \
    else if constexpr ( std::is_same_v<type, wchar_t> ) {   \
        return L ## s;  \
    }   \
    else if constexpr ( std::is_same_v<type, char8_t> ) {   \
        return u8 ## s; \
    }   \
    else if constexpr ( std::is_same_v<type, char16_t> ) {  \
        return u ## s;  \
    }   \
    else if constexpr ( std::is_same_v<type, char32_t> ) {  \
        return U ## s;  \
    }   \
    else {  \
        return s;   \
    }   \
}()
#else
#define __LITERAL(type, s)  \
[]() {  \
    if constexpr ( std::is_same_v<type, char> ) {   \
        return s;   \
    }   \
    else if constexpr ( std::is_same_v<type, wchar_t> ) {   \
        return L ## s;  \
    }   \
    else if constexpr ( std::is_same_v<type, char16_t> ) {  \
        return u ## s;  \
    }   \
    else if constexpr ( std::is_same_v<type, char32_t> ) {  \
        return U ## s;  \
    }   \
    else {  \
        return s;   \
    }   \
}()
#endif

/**
 * @brief Define logical operators for all integral types and a specified enumeration type.
 * 
 * This macro defines logical operators (such as '&&' and '||') between the specified enumeration type 
 * and all integral types (char, short, int, long, etc.). The operators are implemented based on the provided logical functions.
 * 
 * @param Enum_t The enumeration type for which the logical operators are defined.
 * 
 * Example Usage:
 * @code
 * // Define logical operators for MyEnum and all integral types.
 * DEFINE_ENUM_LOGICAL_OP_ALL(MyEnum)
 * // Usage: bool result = (MyEnum::Value && 42);
 * @endcode
 */
#define DEFINE_ENUM_LOGICAL_OP_ALL(Enum_t)    \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, bool)    \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, char) \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, signed char) \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, unsigned char) \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, short) \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, unsigned short) \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, int) \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, unsigned) \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, long) \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, unsigned long) \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, long long) \
    DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, unsigned long long)  \
    DEFINE_ENUM_BINARY_LOGICAL_OP(Enum_t, Enum_t)   \
    DEFINE_UNARY_OP(Enum_t, !, [](auto arg) { return !static_cast< std::underlying_type_t<Enum_t> >(arg); })

/**
 * @brief Define comparison operators for all integral types and specified enumeration type.
 * 
 * This macro defines comparison operators (such as '==', '!=', '<', '<=', '>', '>=', etc.)
 * between the specified enumeration type and all integral types (char, short, int, long, etc.). 
 * The operators are implemented based on the provided comparison functions.
 * 
 * @param Enum_t The enumeration type for which the comparison operators are defined.
 * 
 * Example Usage:
 * @code
 * // Define comparison operators for MyEnum and all integral types.
 * DEFINE_ENUM_COMPARE_OP_ALL(MyEnum)
 * // Usage: bool result = MyEnum::Value == 42;
 * @endcode
 */
#define DEFINE_ENUM_COMPARE_OP_ALL(Enum_t)    \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, char) \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, signed char) \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, unsigned char)   \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, short) \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, unsigned short)  \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, int) \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, unsigned int)    \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, long) \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, unsigned long)   \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, long long)   \
    DEFINE_ENUM_INT_COMPARE_OP(Enum_t, unsigned long long)  \
    DEFINE_ENUM_BINARY_COMPARE_OP(Enum_t, Enum_t)

/**
 * @brief Define logical operators between an enumeration type and an integral type.
 * 
 * This macro defines logical operators (such as '&&' and '||') between the specified enumeration type 
 * and the provided integral type. The operators are implemented based on the provided logical functions.
 * 
 * @param Enum_t The enumeration type for which the logical operators are defined.
 * @param int_t The integral type for the logical operators.
 * 
 * Example Usage:
 * @code
 * // Define custom logical operations for enum type MyEnum and int.
 * DEFINE_ENUM_INT_LOGICAL_OP(MyEnum, int)
 * // Usage: bool result = (MyEnum::Value && 42);
 * @endcode
 */
#define DEFINE_ENUM_INT_LOGICAL_OP(Enum_t, int_t) \
    DEFINE_ENUM_INT_OP(Enum_t, int_t, &, [](auto lhs, auto rhs) { return lhs & rhs; })  \
    DEFINE_ENUM_INT_OP(Enum_t, int_t, |, [](auto lhs, auto rhs) { return lhs | rhs; })  \
    DEFINE_ENUM_INT_OP(Enum_t, int_t, &&, [](auto lhs, auto rhs) { return lhs && rhs; })    \
    DEFINE_ENUM_INT_OP(Enum_t, int_t, ||, [](auto lhs, auto rhs) { return lhs || rhs; })
    
/**
 * @brief Define binary logical operators for two enumeration types.
 * 
 * This macro defines binary logical operators (such as '&&' and '||') between two specified enumeration types.
 * The operators are implemented based on the provided logical functions.
 * 
 * @param Enum1_t The first enumeration type for the logical operators.
 * @param Enum2_t The second enumeration type for the logical operators.
 * 
 * Example Usage:
 * @code
 * // Define custom logical operations for enum types MyEnum1 and MyEnum2.
 * DEFINE_ENUM_BINARY_LOGICAL_OP(MyEnum1, MyEnum2)
 * // Usage: bool result = (MyEnum1::Value && MyEnum2::Value);
 * @endcode
 */
#define DEFINE_ENUM_BINARY_LOGICAL_OP(Enum1_t, Enum2_t) \
    DEFINE_ENUM_BINARY_OP(Enum1_t, Enum2_t, &, [](auto lhs, auto rhs) { return lhs & rhs; })    \
    DEFINE_ENUM_BINARY_OP(Enum1_t, Enum2_t, |, [](auto lhs, auto rhs) { return lhs | rhs; })    \
    DEFINE_ENUM_BINARY_OP(Enum1_t, Enum2_t, &&, [](auto lhs, auto rhs) { return lhs && rhs; })    \
    DEFINE_ENUM_BINARY_OP(Enum1_t, Enum2_t, ||, [](auto lhs, auto rhs) { return lhs || rhs; })    \

/**
 * @brief Define a comparison operator between an enumeration type and an integral type.
 * 
 * This macro defines a comparison operator (such as '==', '!=', '<', '<=', '>', '>=', etc.)
 * between the specified enumeration type and the provided integral type. The operator is implemented based on the provided comparison function.
 * 
 * @param Enum_t The enumeration type for which the comparison operator is defined.
 * @param int_t The integral type to perform the comparison with the enumeration type.
 * @param opSymbol The symbol representing the comparison operator (e.g., '==', '!=', '<', '<=', '>', '>=', etc.).
 * @param opIntFunc The comparison function to be applied to the enumeration and integral values.
 * 
 * Example Usage:
 * @code
 * // Define a custom comparison operation for MyEnum and int.
 * DEFINE_ENUM_INT_COMPARE_OP(MyEnum, int, ==, [](auto lhs, auto rhs) { return lhs == rhs; })
 * // Usage: bool result = MyEnum::Value == 42;
 * @endcode
 */
#define DEFINE_ENUM_INT_COMPARE_OP(Enum_t, int_t)   \
    DEFINE_ENUM_INT_OP(Enum_t, int_t, <=>, [](auto lhs, auto rhs) { return lhs <=> rhs; })

/**
 * @brief Define a comparison operator between two enumeration types.
 * 
 * This macro defines a comparison operator (such as '==', '!=', '<', '<=', '>', '>=', etc.)
 * between the specified enumeration types. The operator is implemented based on the provided comparison function.
 * 
 * @param Enum1_t The first enumeration type for the comparison operator.
 * @param Enum2_t The second enumeration type for the comparison operator.
 * @param opSymbol The symbol representing the comparison operator (e.g., '==', '!=', '<', '<=', '>', '>=', etc.).
 * @param opFunc The comparison function to be applied to the two enumeration values.
 * 
 * Example Usage:
 * @code
 * // Define a custom comparison operation for enum types MyEnum1 and MyEnum2.
 * DEFINE_ENUM_BINARY_COMPARE_OP(MyEnum1, MyEnum2, ==, [](auto lhs, auto rhs) { return lhs == rhs; })
 * // Usage: bool result = MyEnum1::Value == MyEnum2::Value;
 * @endcode
 */
#define DEFINE_ENUM_BINARY_COMPARE_OP(Enum1_t, Enum2_t) \
    DEFINE_ENUM_BINARY_OP(Enum1_t, Enum2_t, <=>, [](auto lhs, auto rhs) { return lhs <=> rhs; })

/**
 * @brief Define a binary operator between two enumeration types.
 * 
 * This macro defines a binary operator (such as '+', '-', '*', '/', etc.) between the specified enumeration types.
 * The operator is implemented based on the provided binary operation function.
 * 
 * @param Enum1_t The first enumeration type for the binary operator.
 * @param Enum2_t The second enumeration type for the binary operator.
 * @param opSymbol The symbol representing the binary operator (e.g., '+', '-', '*', '/', etc.).
 * @param opFunc The binary operation function to be applied to the two enumeration values.
 * 
 * Example Usage:
 * @code
 * // Define a custom binary operation for enum types MyEnum1 and MyEnum2.
 * DEFINE_ENUM_BINARY_OP(MyEnum1, MyEnum2, +, [](auto lhs, auto rhs) { return lhs + rhs; })
 * // Usage: int result = MyEnum1::Value + MyEnum2::Value;
 * @endcode
 */
#define DEFINE_ENUM_BINARY_OP(Enum1_t, Enum2_t, opSymbol, opIntFunc)  \
    DEFINE_BINARY_OP(Enum1_t, Enum2_t, opSymbol,  \
        [](auto e1, auto e2) { return opIntFunc( static_cast< std::underlying_type_t<Enum1_t> >(e1),  \
            static_cast< std::underlying_type_t<Enum2_t> >(e2)    \
        ); } \
    )

/**
 * @brief Define a binary operator between an enumeration type and an integral type.
 * 
 * This macro defines a binary operator (such as '+', '-', '*', '/', etc.) between the specified enumeration type
 * and the provided integral type. The operator is implemented based on the provided binary operation function.
 * 
 * @param Enum_t The enumeration type for which the binary operator is defined.
 * @param int_t The integral type to perform the binary operation with the enumeration type.
 * @param opSymbol The symbol representing the binary operator (e.g., '+', '-', '*', '/', etc.).
 * @param opIntFunc The binary operation function to be applied to the enumeration and integral values.
 * 
 * Example Usage:
 * @code
 * // Define a custom binary operation for MyEnum and int.
 * DEFINE_ENUM_INT_OP(MyEnum, int, +, [](auto lhs, auto rhs) { return lhs + rhs; })
 * // Usage: MyEnum result = MyEnum::Value + 42;
 * @endcode
 */
#define DEFINE_ENUM_INT_OP(Enum_t, int_t, opSymbol, opIntFunc)   \
    DEFINE_BINARY_OP(Enum_t, int_t, opSymbol,   \
        [](auto e, auto i) { return opIntFunc( static_cast<decltype(i)>(e), i ); } \
    )   \
    DEFINE_BINARY_OP(int_t, Enum_t, opSymbol,    \
        [](auto i, auto e) { return opIntFunc( i, static_cast<decltype(i)>(e) ); } \
    )

/**
 * @brief Define a binary operator for enumeration types.
 * 
 * This macro defines a binary operator (such as '+', '-', '*', '/', etc.) for the specified enumeration types.
 * The operator is implemented based on the provided binary operation function.
 * 
 * @param Lhs_t The left-hand side enumeration type for the binary operator.
 * @param Rhs_t The right-hand side enumeration type for the binary operator.
 * @param opSymbol The symbol representing the binary operator (e.g., '+', '-', '*', '/', etc.).
 * @param opFunc The binary operation function to be applied to the enumeration values.
 * 
 * Example Usage:
 * @code
 * // Define a custom binary operation for enum types MyEnum1 and MyEnum2.
 * DEFINE_BINARY_OP(MyEnum1, MyEnum2, +, [](auto lhs, auto rhs) { return lhs + rhs; })
 * // Usage: MyEnum1 result = MyEnum1::Value + MyEnum2::Value;
 * @endcode
 */
#define DEFINE_BINARY_OP(Lhs_t, Rhs_t, opSymbol, opFunc)    \
    static decltype(auto) operator opSymbol (Lhs_t lhs, Rhs_t rhs)   \
        noexcept( noexcept(opFunc(  \
            std::forward<Lhs_t>(lhs),   \
            std::forward<Rhs_t>(rhs)    \
        )) ) {  \
        return opFunc( std::forward<Lhs_t>(lhs),    \
            std::forward<Rhs_t>(rhs) ); \
    }

/**
 * @brief Define a unary operator for an enumeration type.
 * 
 * This macro defines a unary operator (such as '+', '-', '~', etc.) for the specified enumeration type.
 * The operator is implemented based on the provided unary operation function.
 * 
 * @param Arg_t The enumeration type for which the unary operator is defined.
 * @param opSymbol The symbol representing the unary operator (e.g., '+', '-', '~', etc.).
 * @param opFunc The unary operation function to be applied to the enumeration value.
 * 
 * Example Usage:
 * @code
 * // Define a custom unary operation for the enum type MyEnum.
 * DEFINE_UNARY_OP(MyEnum, ~, [](auto arg) { return ~static_cast<std::underlying_type_t<MyEnum>>(arg); })
 * // Usage: MyEnum value = ~MyEnum::Value;
 * @endcode
 */
#define DEFINE_UNARY_OP(Arg_t, opSymbol, opFunc) \
    static decltype(auto) operator opSymbol (Arg_t arg)    \
        noexcept( noexcept( opFunc( \
            std::forward<Arg_t>(arg)    \
        )) ) {  \
        return opFunc( std::forward<Arg_t>(arg) );  \
    }

/**
 * @brief The namespace containing the core functionalities for command-line parameter parsing.
 * 
 * The `gclp` namespace provides fundamental components for parsing command-line arguments, including
 * the `gclp::basic_cl_param` class for defining individual command-line parameters and the
 * `gclp::basic_cl_parser` class for parsing command-line arguments. Additionally, it defines the `gclp::error_code`
 * enum class for representing different error scenarios, and various type aliases like `gclp::parser`,
 * `gclp::optional`, `gclp::required`, `gclp::woptional`, and `gclp::wrequired` for ease of use.
 * 
 * Usage Example:
 * @code
 * // example input1: sorter -i .\\ints.txt
 * // example input2: sorter --in ".\\ints.txt" --out ".\\sorted_ints.txt" --reverse
 * 
 * int main(int argc, char** argv) {
 *     // Define a simple command-line parser for a fictional "file sorter" application.
 *     auto sorterParser = gclp::parser(
 *         "sorter",
 *         gclp::required<std::filesystem::path>(
 *             {'i'}, {"input", "in"}, "Input file path for sorting."
 *         ),
 *         gclp::optional<std::filesystem::path>(
 *             {'o'}, {"output", "out"}, "Output file path. If not provided, prints to console."
 *         ),
 *         gclp::optional<bool>(
 *             {'r'}, {"reverse"}, "Sort in reverse order."
 *         )->defval(false) // set default value
 *     );
 *     
 *     // Parse command-line arguments.
 *     auto [inputFile, outputFile, reverseSort] = sorterParser.parse(argc, argv);
 *     if (sorterParser.error()) {
 *         std::cerr << "Error: " << sorterParser.error_message();
 *         return 1;
 *     }
 *     
 *     // Perform sorting based on parsed parameters...
 * }
 * @endcode
 */
namespace gclp {

#ifndef DOXYGEN_IGNORE_DETAIL
/**
 * @brief The namespace encapsulating internal functionalities for command-line parameter parsing.
 * 
 * The `gclp::detail` namespace provides certain concepts related to instancing `gclp`'s classes,
 * generic string operations, and generic literals.
 */
namespace detail {

/**
 * @brief Concept to check if a type is one of the specified types.
 * 
 * This concept checks if a given type T is the same as any of the types specified in Args.
 * @tparam T The type to be checked against Args.
 * @tparam Args Variadic template arguments representing types to check against.
 */
template <class T, class ... Args>
concept contains = (std::is_same_v<T, Args> || ...);

/**
 * @brief Struct to check if a type is a character type.
 * 
 * This struct checks if a given type T is one of the standard character types.
 * @tparam T The type to be checked.
 */
template <class>
struct is_char : std::false_type
{};

/**
 * @brief Specialization of is_char struct for character types.
 * 
 * This specialization checks if a given type T is one of the standard character types:
 * char, wchar_t, char8_t, char16_t, char32_t, signed char, or unsigned char.
 * @tparam T The type to be checked.
 */
template <class T>
requires contains<T, char, wchar_t, char8_t, char16_t, char32_t,
    signed char, unsigned char>
struct is_char<T> : std::true_type
{};

/**
 * @brief Variable template to check if a type is a character type.
 * 
 * This variable template holds true if the provided type T is a standard character type,
 * and false otherwise.
 * @tparam T The type to be checked.
 */
template <class T>
inline constexpr bool is_char_v = is_char<T>::value;

/**
 * @brief Concept to check if a type is not convertible to another type.
 * 
 * This concept checks if a given type From is not convertible to type To.
 * @tparam From The type to be checked for non-convertibility.
 * @tparam To The target type to which conversion is not allowed.
 */
template <class From, class To>
concept not_convertible_to = !std::is_convertible_v<From, To>;

/**
 * @brief Applies a given function to each argument in a parameter pack.
 *
 * This function template applies the provided function to each argument in the parameter pack,
 * forwarding the arguments to the function. It uses fold expression (C++17) to achieve the iteration.
 *
 * @tparam Func A callable object type.
 * @tparam Args Variadic template parameter pack.
 * @param func A function or callable object to be applied to each argument.
 * @param args Arguments to be passed to the function.
 *
 * @code{.cpp}
 * // Example usage of for_each_args:
 * void print(int num, const std::string& str) {
 *     std::cout << num << ": " << str << std::endl;
 * }
 *
 * int main() {
 *     for_each_args(print, 1, "hello", 2, "world");
 *     return 0;
 * }
 * // Output: 1: hello
 * //         2: world
 * @endcode
 */
template <class Func, class ... Args>
void for_each_args(Func func, Args&& ... args) {
    ( func( std::forward<Args>(args) ), ... );
}

template <class Func, class TupleLike, std::size_t ... Idxs>
void tuple_for_each_helper(
    TupleLike&& t, Func func, std::index_sequence<Idxs...>
) {
    for_each_args( func, std::get<Idxs>(t)... );
}

/**
 * @brief Applies a given function to each element in a tuple-like object.
 *
 * This function template applies the provided function to each element in the tuple-like object,
 * such as std::tuple, std::pair, or user-defined classes with tuple-like behavior.
 *
 * @tparam Func A callable object type.
 * @tparam TupleLike Type of the tuple-like object.
 * @param t The tuple-like object to iterate over.
 * @param func A function or callable object to be applied to each element.
 *
 * @code{.cpp}
 * // Example usage of tuple_for_each:
 * void print(const std::string& str) {
 *     std::cout << str << std::endl;
 * }
 *
 * int main() {
 *     std::tuple<std::string, int, double> myTuple = {"hello", 42, 3.14};
 *     tuple_for_each(myTuple, print); // Output: hello
 *                                    //         42
 *                                    //         3.14
 *     return 0;
 * }
 * @endcode
 */
template <class Func, class TupleLike>
void tuple_for_each(TupleLike&& t, Func func) {
    tuple_for_each_helper( std::forward<TupleLike>(t), func,
        std::make_index_sequence<
            std::tuple_size_v<
                std::remove_cvref_t<TupleLike>
            >
        >{}
    );
}

template <
    class InTupleLike, class OutTupleLike,
    class Func, std::size_t ... Idxs
>
void tuple_conv_helper(
    const InTupleLike& tuple_in, OutTupleLike& tuple_out,
    Func func, std::index_sequence<Idxs...>
) {
    (   ( std::get<Idxs>(tuple_out) = func(
                std::get<Idxs>(tuple_in)
            )
        ), ...
    );
}

/**
 * @brief Converts elements of an input tuple-like object to an output tuple-like object using a given function.
 *
 * This function template applies the provided function to each element in the input tuple-like object
 * and stores the results in the corresponding positions of the output tuple-like object.
 * Both input and output tuple-like objects must have the same number of elements.
 *
 * @tparam InTupleLike Type of the input tuple-like object.
 * @tparam OutTupleLike Type of the output tuple-like object.
 * @tparam Func A callable object type.
 * @param tuple_in The input tuple-like object to be converted.
 * @param tuple_out The output tuple-like object where converted elements will be stored.
 * @param func A function or callable object to transform each input element.
 *
 * @code{.cpp}
 * // Example usage of tuple_conv:
 * std::string add_hello(const std::string& str) {
 *     return "hello " + str;
 * }
 *
 * int main() {
 *     std::tuple<std::string, std::string, std::string> inputTuple = {"world", "there", "everyone"};
 *     std::tuple<std::string, std::string, std::string> outputTuple;
 *     
 *     tuple_conv(inputTuple, outputTuple, add_hello);
 *     
 *     // Output tuple: {"hello world", "hello there", "hello everyone"}
 *     
 *     return 0;
 * }
 * @endcode
 */
template <class InTupleLike, class OutTupleLike, class Func>
    requires requires {
        std::tuple_size_v<
            std::remove_cvref_t<InTupleLike>
        >;
        std::tuple_size_v<
            std::remove_cvref_t<OutTupleLike>
        >;
    }
void tuple_conv(
    const InTupleLike& tuple_in, OutTupleLike& tuple_out,
    Func func
) {
    static_assert(
        std::tuple_size_v<std::remove_cvref_t<InTupleLike>>
        == std::tuple_size_v<std::remove_cvref_t<OutTupleLike>>
    );
    
    tuple_conv_helper(
        tuple_in, tuple_out, func,
        std::make_index_sequence<
            std::tuple_size_v<
                std::remove_cvref_t<InTupleLike>
            >
        >{}
    );
}

/**
 * @brief Generates a dash character.
 * 
 * This function returns the dash character ('-') of the specified type CharT.
 * @tparam CharT The character type.
 * @return The dash character.
 */
template <class CharT>
constexpr CharT dash() {
    return CharT('-');
}

/**
 * @brief Generates a character escape character.
 * 
 * This function returns the character escape character ('\\') of the specified type CharT.
 * @tparam CharT The character type.
 * @return The character escape character.
 */
template <class CharT>
constexpr CharT char_escape() {
    return CharT('\\');
}

/**
 * @brief Generates a stream delimiter character.
 * 
 * This function returns the stream delimiter character (' ') of the specified type CharT.
 * @tparam CharT The character type.
 * @return The stream delimiter character.
 */
template <class CharT>
constexpr CharT stream_delim() {
    return CharT(' ');
}

/**
 * @brief Generates a character assignment character.
 * 
 * This function returns the character assignment character ('=') of the specified type CharT.
 * @tparam CharT The character type.
 * @return The character assignment character.
 */
template <class CharT>
constexpr CharT char_assign() {
    return CharT('=');
}

/**
 * @brief Generates a single quote character.
 * 
 * This function returns the single quote character ('\'') of the specified type CharT.
 * @tparam CharT The character type.
 * @return The single quote character.
 */
template <class CharT>
constexpr CharT single_quote() {
    return CharT('\'');
}

/**
 * @brief Generates a double quote character.
 * 
 * This function returns the double quote character ('\"') of the specified type CharT.
 * @tparam CharT The character type.
 * @return The double quote character.
 */
template <class CharT>
constexpr CharT double_quote() {
    return CharT('\"');
}

/**
 * @brief Checks if the input string represents a single dashed option (e.g., "-x").
 *
 * A single dashed option consists of a dash followed by a non-dash character.
 *
 * @tparam StringView Type of the input string view.
 * @param word The input string view to be checked.
 * @return `true` if the input string represents a single dashed option, `false` otherwise.
 */
template <class StringView>
bool is_single_dashed(StringView word) noexcept {
    using char_type = typename StringView::value_type;

    return std::size(word) > std::size_t(1)
        && word[0] == dash<char_type>()
        && word[1] != dash<char_type>();
}

/**
 * @brief Checks if the input string represents a key (e.g., "-k" or "--key").
 *
 * A key can be either a short key (a dash followed by a non-dash character) or a long key
 * (two dashes followed by one or more non-dash characters).
 *
 * @tparam StringView Type of the input string view.
 * @param word The input string view to be checked.
 * @return `true` if the input string represents a key, `false` otherwise.
 */
template <class StringView>
bool is_key(StringView word) noexcept {
    using char_type = typename StringView::value_type;

    if ( std::size(word) == std::size_t(1) ) {
        return false;
    }
    
    if ( std::size(word) > std::size_t(2) ) {
        return word[0] == dash<char_type>()
            && word[2] != dash<char_type>();
    }
    
    return word[0] == dash<char_type>()
        && word[1] != dash<char_type>();
}

/**
 * @brief Checks if the input string represents a complex key, which means the concatenation of short keys. (e.g., "-abc").
 *
 * @tparam StringView Type of the input string view.
 * @param word The input string view to be checked.
 * @return `true` if the input string represents a complex key, `false` otherwise.
 */
template <class StringView>
bool is_complex_key(StringView word) noexcept {
    return is_single_dashed(word) && std::size(word) > 2;
}

/**
 * @brief Removes leading dashes from the input string view.
 *
 * This function removes any leading dashes from the input string view and returns the modified view.
 * If once faced other characters, following dashes are not removed.
 * It means, if the input appears to be dash-other characters-dash, only the front dashes will be removed.
 *
 * @tparam StringView Type of the input string view.
 * @param s The input string view from which leading dashes will be removed.
 * @return The input string view without leading dashes.
 */
template <class StringView>
StringView remove_dash(StringView s) noexcept {
    return StringView(
        s.begin() + s.find_first_not_of('-'),
        s.end()
    );
}

/**
 * @brief Splits a string view into words using a delimiter.
 *
 * This function splits the input string view into words based on the delimiter,
 * handling escape sequences within quotes. It recognizes single and double quotes as
 * quoting characters and supports escape sequences for the delimiter within quotes.
 *
 * @tparam StringView Type of the input string view.
 * @param s The input string view to be split into words.
 * @return A vector of string views containing the split words.
 *
 * @code{.cpp}
 * // Example usage of split_words:
 * std::string input = "word1 'quoted word2' word3";
 * auto words = split_words(input);
 * // Resulting words: {"word1", "quoted word2", "word3"}
 * @endcode
 */
template <class StringView, class String
    = std::basic_string<
        typename StringView::value_type,
        typename StringView::traits_type
    >
>
std::vector<StringView> split_words(StringView s,
    const StringView delims = String{
        stream_delim< typename StringView::value_type >()
    },
    const StringView quotes = String{
        single_quote<typename StringView::value_type>(),
        double_quote<typename StringView::value_type>()
    }
) {
    using char_type = typename StringView::value_type;

    auto ret = std::vector<StringView>{};

    auto it_first = std::begin(s);
    auto it_last = std::end(s);
    auto it_out = std::back_inserter(ret);

    auto cur_delims = delims;

    auto is_delim = [&cur_delims](auto args) {
            return cur_delims.find_first_of(args) != StringView::npos;
    };
    auto new_delim = [delims, quotes](auto ch)
        -> StringView {
        if ( quotes.find_first_of(ch) != StringView::npos ) {
            return String{ ch };
        }
        else {
            return delims;
        }
    };

    for (; it_first != it_last; ++it_first) {
        it_first = std::ranges::find_if_not(it_first, it_last, is_delim);
        if (it_first == it_last) {
            break;
        }
        cur_delims = new_delim(*it_first);
        
        it_first = std::ranges::find_if_not(it_first, it_last, is_delim);
        if (it_first == it_last) {
            break;
        }
        
        auto it_split_last = std::ranges::find_if(
            it_first, it_last, is_delim
        );
        it_out = StringView(it_first, it_split_last);
        if (it_split_last == it_last) {
            break;
        }
        cur_delims = new_delim(*it_first);
        it_first = it_split_last;
    }

    return ret;
}

template <class Param>
class param_ctor_proxy {
public:
    using parent_type = Param;  ///< Alias for the parent parameter type.
    using value_type = typename parent_type::value_type;    ///< Alias for the value type of the parent parameter.

    /**
     * @brief Constructs a param_ctor_proxy with a pointer to the parent parameter.
     *
     * @param parent A pointer to the parent parameter for which default values will be adapted.
     */
    param_ctor_proxy(parent_type* parent)
        : parent_(parent) {}

    /**
     * @brief Sets a new default value for the parent parameter.
     *
     * @param val The new default value to be set.
     * @return A reference to the parent parameter with the updated default value.
     */
    parent_type& defval(const value_type& val) {
        parent_->set_defval(val);
        return *parent_;
    }

    /**
     * @brief Sets a new default value for the parent parameter using move semantics.
     *
     * @param val The new default value to be set.
     * @return A reference to the parent parameter with the updated default value.
     */
    parent_type& defval(value_type&& val) {
        parent_->set_defval( std::move(val) );
        return *parent_;
    }

private:
    parent_type* parent_;
};

}   // namespace gclp::detail
#endif   // DOXYGEN_IGNORE_DETAIL

/**
 * @brief Concept representing character types suitable for command-line.
 * 
 * This concept checks if the provided type T satisfies the requirements for a command-line character.
 * @tparam T The character type to be checked.
 */
template <class T>
concept gclp_char = detail::is_char_v<T>;

/**
 * @brief Represents a command line parameter with specified short keys, long keys, and a brief description.
 *
 * This class template represents a command line parameter that can be identified by a set of short keys,
 * long keys, and a brief description. It allows setting a value, default value, and provides functionality
 * to check for key presence, assign values, and retrieve brief descriptions.
 *
 * @tparam ValT Type of the parameter value.
 * @tparam CharT Character type for keys (default is char).
 * @tparam Traits Character traits type for keys (default is std::char_traits<CharT>).
 */
template <
    class ValT, gclp_char CharT = char,
    class Traits = std::char_traits<CharT>
>
class basic_cl_param
{
public:
    using value_type = ValT;
    using char_type = CharT;
    using traits_type = Traits;
    using string_view_type =
        std::basic_string_view<char_type, traits_type>;
    template <class T>
    using key_container = std::vector<T>;

private:
    using istream_type = std::basic_istream<char_type, traits_type>;
    using ostream_type = std::basic_ostream<char_type, traits_type>;

public:
    /**
     * @brief Constructs a basic_cl_param object with specified short keys, long keys, and a brief description.
     *
     * @param short_keys Set of short keys representing the parameter.
     * @param long_keys Set of long keys representing the parameter.
     * @param brief Brief description of the parameter.
     */
    basic_cl_param(
        std::initializer_list<char_type> short_keys,
        std::initializer_list<string_view_type> long_keys,
        string_view_type brief
    ) : short_keys_(short_keys), long_keys_(long_keys),
        brief_(brief), defval_(), val_(), fail_(false) {}

    /**
     * @brief Checks if the parameter has a value (either set or default).
     *
     * @return `true` if the parameter has a value, `false` otherwise.
     */
    bool has_value() const noexcept {
        return val_.has_value() || defval_.has_value();
    }

    /**
     * @brief Retrieves the value of the parameter.
     *
     * @return Reference to the value of the parameter.
     */
    value_type& value() noexcept {
        assert(has_value());

        if (val_.has_value()) {
            return val_.value();
        }
        return defval_.value();
    }

    /**
     * @brief Retrieves the value of the parameter (const version).
     *
     * @return Const reference to the value of the parameter.
     */
    const value_type& value() const noexcept {
        assert(has_value());

        if (val_.has_value()) {
            return val_.value();
        }
        return defval_.value();
    }

    /**
     * @brief Removes the value of the parameter.
     * 
     * if it has no value before, this function does nothing.
     */
    void remove_value() noexcept {
        val_.reset();
    }

    /**
     * @brief Checks if the parameter contains a specific short key character.
     *
     * @param short_key Short key character to check for.
     * @return `true` if the parameter contains the short key character, `false` otherwise.
     */
    bool contains(char_type short_key) const {
        return std::ranges::find(short_keys_, short_key) != std::end(short_keys_);
    }
    
    /**
     * @brief Checks if the parameter contains a specific long key string.
     *
     * @param long_key Long key string to check for.
     * @return `true` if the parameter contains the long key string, `false` otherwise.
     */
    bool contains(string_view_type long_key) const {
        return std::ranges::find(long_keys_, long_key) != std::end(long_keys_);
    }
    
    /**
     * @brief Checks if the parameter contains a long key specified by a range of characters.
     *
     * @tparam It Iterator type representing the range of characters.
     * @param first Iterator pointing to the start of the character range.
     * @param last Iterator pointing to the end of the character range.
     * @return `true` if the parameter contains the specified key, `false` otherwise.
     */
    template <std::input_iterator It>
        requires requires(It a, It b) {
            string_view_type(a, b);
        }
    bool contains(It first, It last) const {
        return std::ranges::find(
                long_keys_, string_view_type(first, last)
            ) != std::end(long_keys_);
    }
    
    /**
     * @brief Checks if the parameter contains a long key specified by a range.
     *
     * @tparam R Range representing the key.
     * @param r Range to check for.
     * @return `true` if the parameter contains the specified key, `false` otherwise.
     */
    template <std::ranges::range R>
       requires std::convertible_to<R, string_view_type>
    bool contains(R&& r) const {
        return contains(
            static_cast<string_view_type>( std::forward<R>(r) )
        );
    }

    /**
     * @brief Retrieves the short keys associated with the parameter.
     *
     * @return Reference to the container containing short keys.
     */
    const key_container<char_type>& short_keys() const noexcept {
        return short_keys_;
    }

    /**
     * @brief Retrieves the long keys associated with the parameter.
     *
     * @return Reference to the container containing long keys.
     */
    const key_container<string_view_type>& long_keys() const noexcept {
        return long_keys_;
    }

    /**
     * @brief Checks if the parameter has a brief description message.
     *
     * @return `true` if the parameter has a brief description, `false` otherwise.
     */
    bool has_brief_message() const noexcept {
        return brief_.has_value();
    }

    /**
     * @brief Retrieves the brief description of the parameter.
     *
     * @pre has_brief_message() should be true.
     * @return Const reference to the brief description of the parameter.
     */
    const string_view_type brief_message() const noexcept {
        assert(has_brief_message());
        return brief_.value();
    }

    /**
     * @brief fail assigning incompatible argument. fail bit is set.
     *
     * @tparam InCompatible Type of the input value.
     * @param arg Input value to be assigned.
     * @return `false` (type mismatch).
     */
    template <class InCompatible>
        requires detail::not_convertible_to< 
            std::remove_cvref_t<InCompatible>,
            value_type
        >
    bool assign(InCompatible&& arg) {
        fail_ = true;
        return false;
    }

    /**
     * @brief Assigns a value to the parameter (const lvalue reference version).
     *
     * @param val Input value to be assigned (const lvalue reference).
     * @return `false` if fail bit is already set, otherwise `true`
     */
    bool assign(const value_type& val) {
        if (fail()) {
            return false;
        }
        val_ = val;
        return true;
    }

    /**
     * @brief Assigns a value to the parameter (rvalue reference version).
     *
     * @param val Input value to be assigned (rvalue reference).
     * @return `false` if fail bit is already set, otherwise `true`
     */
    bool assign(value_type&& val) {
        if (fail()) {
            return false;
        }
        val_ = std::move(val);
        return true;
    }

    /**
     * @brief Checks if a type mismatch occurred during assignment.
     *
     * @return `true` if a type mismatch occurred, `false` otherwise.
     */
    bool fail() const noexcept {
        return fail_;
    }

    /**
     * @brief Clears the fail flag.
     */
    void clear() noexcept {
        fail_ = false;
    }

    /**
     * @brief Output stream operator overload for the parameter.
     *
     * Outputs the parameter's value or default value if set.
     * if the parameter has neither value nor default value, this function does nothing.
     *
     * @param os Output stream.
     * @param p Reference to the basic_cl_param object.
     * @return Reference to the output stream after the operation.
     */
    friend ostream_type& operator<<(
        ostream_type& os, const basic_cl_param& p
    ) {
        if (p.val_.has_value()) {
            os << p.val_.value();
        }
        else if (p.defval_.has_value()) {
            os << p.defval_.value();
        }
        else {
            // no value to output
        }

        return os;
    }

    /**
     * @brief Input stream operator overload for the parameter.
     *
     * Reads and tries assigning a value to the parameter from the input stream.
     * Sets the fail flag if the value is incompatible.
     *
     * @param is Input stream.
     * @param p Reference to the basic_cl_param object.
     * @return Reference to the input stream after the operation.
     */
    friend istream_type& operator>>(
        istream_type& is, basic_cl_param& p
    ) {
        p.val_ = value_type();
        is >> p.val_.value();

        if (is.fail()) {
            // incompatible type received.
            p.fail_ = true;
            is.clear();
        }

        return is;
    }

    /**
     * @brief Sets the default value of the parameter.
     *
     * @param val Default value to be set for the parameter.
     */
    void set_defval(const value_type& val) {
        defval_ = val;
    }

    /**
     * @brief Sets the default value of the parameter (move version).
     *
     * @param val Default value to be set for the parameter (rvalue reference).
     */
    void set_defval(value_type&& val) {
        defval_ = std::move(val);
    }

    /**
     * @brief Checks if the parameter has a default value.
     *
     * @return `true` if the parameter has a default value, `false` otherwise.
     */
    bool has_defval() const noexcept {
        return defval_.has_value();
    }

    /**
     * @brief Retrieves the default value of the parameter.
     *
     * @return Const reference to the default value of the parameter.
     */
    const value_type& get_defval() const {
        assert(has_defval());
        return defval_.value();
    }

    /**
     * @brief Removes the default value of the parameter.
     */
    void remove_defval() noexcept {
        defval_.reset();
    }

private:
    key_container<char_type> short_keys_;
    key_container<string_view_type> long_keys_;
    std::optional<string_view_type> brief_;
    std::optional<value_type> defval_;
    std::optional<value_type> val_;
    bool fail_;
};

/**
 * @brief Represents an optional command line parameter with specified short keys, long keys, and a brief description.
 *
 * This class template represents an optional command line parameter that can be identified by a set of short keys,
 * long keys, and a brief description. It allows setting a value, default value, and provides functionality
 * to check for key presence, assign values, and retrieve brief descriptions. Additionally, it provides a default
 * value adaptor for setting default parameter values.
 *
 * Example Usage:
 * @code
 * // Define an optional integer parameter with short key 'i' and long key "integer".
 * auto optionalParam = gclp::optional<int>{'i', "integer", "An optional integer parameter"};
 *
 * // Check if the parameter was provided in the command line arguments.
 * if (optionalParam.contains("i")) {
 *     // Attempt to assign the parameter value from command line arguments.
 *     if (optionalParam.assign(argv[2])) {
 *         // Assignment was successful; retrieve the assigned value.
 *         int value = optionalParam.value();
 *         // Use the parameter value...
 *     } else {
 *         // Assignment failed; parameter value was not valid.
 *         // Handle accordingly...
 *     }
 * } else {
 *     // Parameter was not provided in command line arguments.
 *     // Handle accordingly...
 * }
 * @endcode
 * 
 * @code
 * // Define an optional boolean parameter with short key 'v' and long key "verbose".
 * auto booleanParam = gclp::optional<bool>{
 *     'v', "verbose", "An optional verbose flag"
 * }->defval(false);
 *
 * // Check if the boolean parameter was provided in the command line arguments.
 * if (booleanParam.contains("v")) {
 *     // Assign the boolean parameter's value (true) since its presence indicates true.
 *     booleanParam.assign(true);
 *     if ( booleanParam.fail() ) {
 *         // Handle assignment failure...
 *     }
 *     else {
 *     // Use the parameter value...
 *     }
 * } else {
 *     // Parameter was not provided in command line arguments; it defaults to false.
 *     // Use the parameter value...
 * }
 * @endcode
 *
 * @tparam ValT Type of the parameter value.
 * @tparam CharT Character type for keys (default is char).
 * @tparam Traits Character traits type for keys (default is std::char_traits<CharT>).
 */
template <class ValT, gclp_char CharT = char,
    class Traits = std::char_traits<CharT>
>
class basic_optional : public basic_cl_param<ValT, CharT, Traits> {
public:
    using value_type = ValT;
    using char_type = CharT;
    using traits_type = Traits;
    using string_view_type = std::basic_string_view<
        char_type, traits_type
    >;

private:
    using defval_adaptor_type = detail::param_ctor_proxy<
        basic_optional<value_type, char_type, traits_type>
    >;
    friend defval_adaptor_type;

public:
    /**
     * @brief Constructs a basic_optional object with specified short keys, long keys, and brief description.
     * 
     * Initializes a basic_optional object with the specified short keys, long keys, and brief description.
     * @param short_keys An std::initializer_list of short keys associated with the parameter.
     * @param long_keys An std::initializer_list of long keys associated with the parameter.
     * @param brief A brief description of the parameter.
     */
    basic_optional(std::initializer_list<char_type> short_keys,
        std::initializer_list<string_view_type> long_keys,
        string_view_type brief
    ) : basic_cl_param<ValT, CharT, Traits>(short_keys, long_keys, brief),
        defval_adaptor_(this) {}

    /**
     * @brief Provides access to the default value adaptor for setting default values of the parameter.
     *
     * This member function allows direct access to the default value adaptor associated with the parameter. It enables
     * setting default values for the parameter using the `defval` member functions provided by the adaptor.
     *
     * Example Usage:
     *
     * @code
     * // Sets the default value of the parameter to 42 using the default value adaptor.
     * auto param = gclp::optional<int>{'i', "integer", "An optional integer parameter"}->defval(42);
     * @endcode
     *
     * @return A pointer to the default value adaptor for the parameter.
     */
    defval_adaptor_type* operator->() && noexcept {
        return &defval_adaptor_;
    }

private:
    defval_adaptor_type defval_adaptor_;
};

/**
 * @brief Represents a required command line parameter with specified short keys, long keys, and a brief description.
 *
 * This class template represents a required command line parameter that must be identified by a set of short keys,
 * long keys, and a brief description. It allows setting a value, default value, and provides functionality
 * to check for key presence, assign values, and retrieve brief descriptions. Additionally, it provides a default
 * value adaptor for setting default parameter values.
 *
 * Example Usage:
 * @code
 * // Define a required path parameter with short key 'f' and long key "file".
 * auto requiredParam = gclp::required<std::filesystem::path>{
 *     'f', "file", "A required file parameter"
 * };
 *
 * // Attempt to assign the parameter value from command line arguments.
 * if (requiredParam.assign(argv[2])) {
 *     // Assignment was successful; retrieve the assigned value.
 *     std::filesystem::path filename = requiredParam.value();
 *     // Use the parameter value...
 * } else {
 *     // Assignment failed; parameter value was not valid.
 *     // Handle accordingly...
 * }
 * @endcode
 *
 * @tparam ValT Type of the parameter value.
 * @tparam CharT Character type for keys (default is char).
 * @tparam Traits Character traits type for keys (default is std::char_traits<CharT>).
 */
template <class ValT, gclp_char CharT = char,
    class Traits = std::char_traits<CharT>
>
class basic_required : public basic_cl_param<ValT, CharT, Traits> {
public:
    using value_type = ValT;
    using char_type = CharT;
    using traits_type = Traits;
    using string_view_type = std::basic_string_view<
        char_type, traits_type
    >;

private:
    using defval_adaptor_type = detail::param_ctor_proxy<
        basic_required<value_type, char_type, traits_type>
    >;
    friend defval_adaptor_type;

public:
    /**
     * @brief Constructs a basic_required object with specified short keys, long keys, and brief description.
     * 
     * Initializes a basic_required object with the specified short keys, long keys, and brief description.
     * @param short_keys An std::initializer_list of short keys associated with the parameter.
     * @param long_keys An std::initializer_list of long keys associated with the parameter.
     * @param brief A brief description of the parameter.
     */
    basic_required(std::initializer_list<char_type> short_keys,
        std::initializer_list<string_view_type> long_keys,
        string_view_type brief
    ) : basic_cl_param<ValT, CharT, Traits>(short_keys, long_keys, brief),
        defval_adaptor_(this) {}

    /**
     * @brief Provides access to the default value adaptor for setting default values of the parameter.
     *
     * This member function allows direct access to the default value adaptor associated with the parameter. It enables
     * setting default values for the parameter using the `defval` member functions provided by the adaptor.
     *
     * Example Usage:
     *
     * @code
     * // Sets the default value of the parameter to 42 using the default value adaptor.
     * auto param = gclp::required<int>{'i', "integer", "A required integer parameter"}->defval(42);
     * @endcode
     *
     * @return A pointer to the default value adaptor for the parameter.
     */
    defval_adaptor_type* operator->() && noexcept {
        return &defval_adaptor_;
    }

private:
    defval_adaptor_type defval_adaptor_;
};

/**
 * @brief Enumeration representing error codes during command-line argument parsing.
 * 
 * This enumeration defines various error states that can occur during the parsing of command-line arguments.
 */
enum class error_code {
    identifier_not_given,   ///< The identifier was not provided, i.e. the command line is empty.
    invalid_identifier, ///< The provided identifier is invalid or malformed.
    key_not_given,      ///< A required key was not provided in the command-line arguments.
    undefined_key,      ///< An undefined or unexpected key was provided in the command-line arguments.
    unparsed_argument,  ///< An argument could not be parsed successfully.
    incompatible_argument, ///< The provided argument is incompatible with the associated key.
    wrong_complex_key,  ///< An incorrect complex key format was provided.
    required_key_not_given, ///< A required key was not provided in the command-line arguments.
    duplicated_assignments  ///< More then one of the keys are assigning value to the same parameter.
};

DEFINE_ENUM_LOGICAL_OP_ALL(error_code)
DEFINE_ENUM_COMPARE_OP_ALL(error_code)

/**
 * @brief A generic command-line parser template for processing command-line arguments.
 *
 * @tparam CharT The character type used in the command-line arguments (defaulted to char).
 * @tparam Traits The character traits type (defaulted to std::char_traits<CharT>).
 * @tparam Params Parameter types that define the expected command-line arguments.
 * 
 * This class template is designed to handle parsing command-line arguments based on a predefined set of parameters.
 * It allows specifying various optional and required parameters, parsing input strings, and validating the
 * provided arguments.
 * 
 * This class template allows seamless parsing of command-line arguments by leveraging class type deduction,
 * providing an elegant and expressive way to handle various types of parameters. It simplifies the process
 * of defining and parsing command-line options, making it intuitive and user-friendly.
 *
 * Example Usage:
 * 
 * @code
 * // example input1: todo -t study -p 1 -d false
 * // example input2: todo --task study --done
 * 
 * int main(int argc, char** argv) {
 *     // Create a parser for a simple todo list application
 *     auto todoParser = gclp::basic_cl_parser(
 *         "todo", // Command name
 *         gclp::optional<std::string>{ "-t", "--task", "Specify the task name to modify,
 *             or add a new task to the todo list." },
 *         gclp::optional<int>{ "-p", "--priority", "Set priority for the task." },
 *         gclp::optional<bool>{ "-d", "--done", "Mark the task as done." }
 *     );
 *     
 *     auto [taskName, taskPriority, isTaskDone] = todoParser.parse(argc, argv);
 *     
 *     if (todoParser.error()) {
 *         std::cerr << "Error: " << todoParser.error_message() << std::endl;
 *         std::terminate();
 *     }
 *     
 *     // Process the parsed task information...
 * }
 * @endcode
 *
 * In the above example, the `todoParser` instance is created to handle command-line arguments for a todo list application.
 * The parser automatically deduces the parameter types, simplifying the code and enhancing readability.
 */
template <
    class CharT = char,
    class Traits = std::char_traits<CharT>,
    class ... Params
>
class basic_cl_parser
{
public:
    using char_type = CharT;    /**< The character type used in the command-line arguments. */
    using traits_type = Traits; /**< The character traits type for string manipulations. */
    using string_type = std::basic_string<char_type, traits_type>;  /**< Type representing strings. */
    using string_view_type = std::basic_string_view<char_type, traits_type>;    /**< Type representing string views. */
    using result_tuple_type = std::tuple<typename Params::value_type...>;   /**< Type representing parsed parameter values. */
    using param_tuple_type = std::tuple<Params...>; /**< Type representing the parameter tuple. */
    using index_type = int; /**< Type representing parameter indices. */
    using stream_type = std::basic_stringstream<char_type, traits_type>;    /**< Type representing string streams. */
    using words_type = std::vector<string_view_type>;   /**< Type representing splitted command-line words. */

private:

/**
 * @brief Internal container class for managing parsed parameter data.
 *
 * The `container` class is responsible for storing and managing the parsed parameter data. It maintains the parameter
 * tuple (`data_`) containing instances of parameter objects, and a cached result tuple (`cached_values_`) representing
 * the parsed values of the parameters. The container class provides methods to access and manipulate the parameter data
 * and ensures efficient caching of parsed values.
 */
class container {
public:
    /**
     * @brief Constructs a container with the specified parameter objects.
     *
     * Initializes the container with the provided parameter objects. The parameter objects are moved into the container.
     *
     * @param params Parameter objects representing the expected command-line arguments.
     */
    container(Params&& ... params)
        : data_( std::move(params)... ),
        cached_values_() {}

    /**
     * @brief Retrieves a mutable reference to the parameter tuple.
     *
     * @return A mutable reference to the parameter tuple containing parameter objects.
     */
    param_tuple_type& data() noexcept {
        return data_;
    }

    /**
     * @brief Retrieves a constant reference to the parameter tuple.
     *
     * @return A constant reference to the parameter tuple containing parameter objects.
     */
    const param_tuple_type& data() const noexcept {
        return data_;
    }

    /**
     * @brief Retrieves a mutable reference to the cached parsed values.
     *
     * If the cached values are not available, this method triggers an update of the cache and returns the mutable
     * reference to the cached parsed values.
     *
     * @return A mutable reference to the parsed values obtained from the parameters.
     */
    result_tuple_type& values() {
        if (!cached_values_.has_value()) {
            update_cache();
        }
        return cached_values_.value();
    }

    /**
     * @brief Retrieves a constant reference to the cached parsed values.
     *
     * If the cached values are not available, this method triggers an update of the cache and returns the constant
     * reference to the cached parsed values.
     *
     * @return A constant reference to the parsed values obtained from the parameters.
     */
    const result_tuple_type& values() const {
        if (!cached_values_.has_value()) {
            update_cache();
        }
        return cached_values_.value();
    }

    /**
     * @brief Clears the parameter data and invalidates the cached values.
     *
     * This method clears the parameter objects and resets the cached parsed values. It prepares the container for
     * fresh parsing and assignment of new command-line arguments.
     */
    void clear() {
        detail::tuple_for_each(data_,
            [](auto& p) {
                p.clear();
            }
        );
        invalidate_cache();
    }

    /**
     * @brief Invalidates the cached parsed values.
     *
     * This method resets the cached parsed values, indicating that the cached values are no longer valid. Subsequent
     * access to the cached values will trigger an update.
     */
    void invalidate_cache() {
        cached_values_.reset();
    }

    /**
     * @brief Updates the cached parsed values from the parameter data.
     *
     * This method updates the cached parsed values by invoking the `tuple_conv` function to convert parameter objects
     * to the corresponding parsed values. It is called internally when accessing the cached values if the cache is not
     * already available.
     */
    void update_cache() {
        cached_values_ = result_tuple_type();
        detail::tuple_conv(data_, cached_values_.value(),
            [this](const auto& p) {
                using none_qualified_t
                    = std::remove_cvref_t<decltype(p)>;
                using value_type
                    = typename none_qualified_t::value_type;

                if (p.has_value()) {
                    return p.value();
                }

                return value_type{};
            }
        );
    }

private:
    param_tuple_type data_; /**< The tuple storing parameter objects representing command-line arguments. */
    mutable std::optional<result_tuple_type> cached_values_; /**< Optional container for cached parsed values. */
};  // class basic_cl_param::container

/**
 * @brief Interpreter class for processing command-line arguments.
 * The `interpreter` class is responsible for interpreting and tokenizing the command-line arguments. It breaks down the
 * command line into segmented tokens, where each token comprises an initial word (identifier or key) and a collection of
 * following words (arguments) associated with it.
 */
class interpreter {
public:
    using iterator = std::ranges::iterator_t<words_type>;   /**< Type representing iterator for command-line words. */

    /**
     * @brief Structure representing a tokenized segment of the command-line arguments.
     *
     * The `token` struct represents a segmented portion of the command-line arguments. It consists of an initial word
     * (identifier or key) and a collection of following words (arguments) associated with it.
     */
    struct token {
        string_view_type leading; /**< The initial word (identifier or key) extracted from the command line. */
        words_type followings; /**< Collection of following words (arguments) associated with the initial word. */
    };

    interpreter() = default;

    /**
     * @brief Constructor for the interpreter with argc and argv input.
     *
     * Initializes the interpreter by flattening the `argc` and `argv` into a single command line and splitting it into
     * words for processing.
     *
     * @param argc Number of command-line arguments.
     * @param argv Array of strings representing the command-line arguments.
     */
    template <class IntType, class StrArrType>
    interpreter( IntType argc, const StrArrType& argv )
        : src_( flatten_argc_argv(argc, argv) ),
        words_( detail::split_words<string_view_type>(src_.value()) ),
        cur_( std::begin(words_) ) {}

    /**
     * @brief Constructor for the interpreter with a command line input.
     *
     * Initializes the interpreter by splitting the provided `command_line` into words for processing.
     *
     * @param command_line Command-line input as a string view.
     */
    interpreter(string_view_type command_line)
        : src_(), words_( detail::split_words(command_line) ),
        cur_( std::begin(words_) ) {}

    /**
     * @brief Retrieves the next token from the command-line arguments.
     *
     * This function retrieves the next token from the command-line arguments, where a token is a segmented portion
     * comprising an initial word (identifier or key) and a collection of following words (arguments) associated with it.
     *
     * @return A `token` struct representing the next segmented portion of the command-line arguments.
     */
    token get_token() {
        auto ret = token();
        if ( done() ) {
            return ret;
        }

        ret.leading = read();
        auto out = std::back_inserter(ret.followings);

        while ( !(done() || facing_key()) ) {
            out = read();
        }
        
        return ret;
    }

    /**
     * @brief Resets the interpreter's internal state, allowing parsing from the beginning.
     */
    void reset() {
        cur_ = std::begin(words_);
    }

    /**
     * @brief Resets the interpreter's internal state with new command-line arguments.
     *
     * Resets the interpreter's internal state with new command-line arguments provided as `argc` and `argv`.
     *
     * @param argc Number of command-line arguments.
     * @param argv Array of strings representing the command-line arguments.
     */
    template <class IntType, class StrArrType>
    void reset(IntType argc, const StrArrType& argv) {
        src_ = flatten_argc_argv(argc, argv);
        reset( src_.value() );
    }

    /**
     * @brief Resets the interpreter's internal state with a new command line string.
     *
     * Resets the interpreter's internal state with a new command line string provided as `cmd`.
     *
     * @param cmd The new input command line string.
     */
    void reset(string_view_type cmd) {
        words_ = detail::split_words(cmd);
        cur_ = std::begin(words_);
    }

    /**
     * @brief Checks if parsing of command-line arguments is complete.
     *
     * @return `true` if all command-line arguments have been parsed, `false` otherwise.
     */
    bool done() const noexcept {
        return cur_ == std::end(words_);
    }

    /**
     * @brief Checks if the interpreter is currently facing a key in the command line.
     *
     * @return `true` if a key is encountered, `false` otherwise.
     */
    bool facing_key() const {
        return detail::is_key(*cur_);
    }

    /**
     * @brief Returns the number of remaining words in the command-line arguments.
     *
     * @return The number of remaining words in the command-line arguments.
     */
    std::size_t remainder_count() const noexcept {
        return std::distance( cur_, std::end(words_) );
    }

    /**
     * @brief Removes leading dashes from a word and returns the modified view.
     *
     * @param word The input string view from which leading dashes will be removed.
     * @return The input string view without leading dashes.
     */
    string_view_type remove_dash(string_view_type word) const {
        return detail::remove_dash(word);
    }

private:
    /**
     * @brief Flattens the command-line arguments into a single string.
     *
     * Flattens the command-line arguments into a single string, separating words using a delimiter.
     *
     * @tparam IntType Type representing the number of command-line arguments.
     * @tparam StrArrType Type representing the array of strings representing the command-line arguments.
     * @param argc Number of command-line arguments.
     * @param argv Array of strings representing the command-line arguments.
     * @return A string containing all flattened command-line arguments.
     */
    template <class IntType, class StrArrType>
    static string_type flatten_argc_argv(
        IntType argc, const StrArrType& argv
    ) {
        auto flattend = string_type();

        // quote identifier(which is argv[0]) to handle space in the path.
        if (argc > decltype(argc)(0)) {
            auto quoted_identifier = string_type{
                detail::double_quote<char_type>()
            };
            quoted_identifier += string_view_type(argv[0]);
            quoted_identifier += detail::double_quote<char_type>();

            flattend += std::move(quoted_identifier);
            flattend += detail::stream_delim<char_type>();
        }

        for (auto i = decltype(argc)(1); i < argc; ++i) {
            flattend += string_view_type(argv[i]);
            flattend += detail::stream_delim<char_type>();
        }

        return flattend;
    }

    /**
     * @brief Reads and returns the next word from the command-line arguments.
     *
     * Reads and returns the next word from the command-line arguments, advancing the iterator.
     *
     * @return The next word from the command-line arguments.
     */
    string_view_type read() {
        return *cur_++;
    }

    std::optional<string_type> src_;
    words_type words_;
    iterator cur_;
};  // class basic_cl_parser::interpreter

/**
 * @brief A class responsible for verifying the correctness of parsed command-line arguments.
 *
 * The `verifier` class ensures that the parsed command-line arguments are valid, considering required keys,
 * duplicate assignments, correct key formats, and more. It provides methods to check the validity of individual
 * keys, complex keys, and identifiers. It also tracks the state of switches to prevent duplicate assignments and
 * verifies whether all required keys are provided.
 */
class verifier {
public:
    using bitset_type = std::bitset<
        std::tuple_size_v<param_tuple_type>
    >;  /**< Type representing a set of switches for parameters. */

    /**
     * @brief Constructs a `verifier` object with the specified identifier.
     *
     * Initializes the `verifier` object with the given command-line identifier, setting switches and flags to default states.
     *
     * @param identifier The identifier associated with the command-line arguments.
     */
    verifier(string_view_type identifier)
        : identifier_(identifier), switches_(0u),
        fail_(false), bad_(false) {}

    /**
     * @brief Retrieves the identifier associated with the command-line arguments.
     *
     * @return The identifier associated with the command-line arguments.
     */
    string_view_type id() const noexcept {
        return identifier_;
    }

    /**
     * @brief Checks if a given word matches the command-line identifier.
     *
     * This function verifies whether the provided word matches the command-line identifier.
     *
     * @param word The word to be checked.
     * @return `true` if the word matches the identifier, otherwise `false`.
     */
    bool is_valid_identifier(string_view_type word)
        const noexcept {
        auto splitted = detail::split_words(word,
            string_view_type(__LITERAL(char_type, "/\\ "))
        );

        return splitted.back() == identifier_;
    }

    /**
     * @brief Checks if a short key has a duplicated assignment among parameters.
     *
     * This function verifies if the provided short key has a duplicated assignment among the parameters.
     *
     * @param short_key The short key to be checked.
     * @param params The container of parameters to be verified.
     * @return `true` if the short key has a duplicated assignment, otherwise `false`.
     */
    bool is_duplicated_assignment(
        char_type short_key, const container& params
    ) const {
        return is_duplicated_assignment_impl(short_key, params);
    }

    bool is_duplicated_assignment( string_view_type long_key,
        const container& params
    ) const {
        return is_duplicated_assignment_impl(long_key, params);
    }

    /**
     * @brief Checks if a complex key has duplicated assignments among parameters.
     *
     * This function verifies if the provided complex key has duplicated assignments among the parameters.
     *
     * @param keys The complex key to be checked.
     * @param params The container of parameters to be verified.
     * @return `true` if the complex key has duplicated assignments, otherwise `false`.
     */
    bool is_duplicated_complex_assignment(
        string_view_type keys, const container& params
    ) const {
        auto tmp_switches = bitset_type(0u);
        return std::ranges::any_of(keys,
            [&params, &tmp_switches, this](auto key) {
                auto is = this->is_duplicated_assignment_impl(
                    key, params, tmp_switches
                );

                this->set_assigned_impl(
                    key, params, tmp_switches
                );

                return is;
            }
        );
    }

    /**
     * @brief Checks if a token starts with a valid short key format.
     *
     * This function verifies if the leading token of a command-line argument starts with a valid short key format.
     *
     * @param token The token to be checked.
     * @return `true` if the token starts with a valid short key, otherwise `false`.
     */
    bool starts_with_short_key(
        const typename interpreter::token& token
    ) const {
        return detail::is_key( token.leading )
            && detail::is_single_dashed( token.leading )
            && !detail::is_complex_key( token.leading );
    }

    /**
     * @brief Checks if a token starts with a valid long key format.
     *
     * This function verifies if the leading token of a command-line argument starts with a valid long key format.
     *
     * @param token The token to be checked.
     * @return `true` if the token starts with a valid long key, otherwise `false`.
     */
    bool starts_with_long_key(
        const typename interpreter::token& token
    ) const {
        return detail::is_key( token.leading )
            && !detail::is_single_dashed( token.leading );
    }

    /**
     * @brief Checks if a token starts with a valid complex key format.
     *
     * This function verifies if the leading token of a command-line argument starts with a valid complex key format.
     *
     * @param token The token to be checked.
     * @return `true` if the token starts with a valid complex key, otherwise `false`.
     */
    bool starts_with_complex_key(
        const typename interpreter::token& token
    ) const {
        return detail::is_complex_key( token.leading );
    }

    /**
     * @brief Checks if a single short key is valid among parameters.
     *
     * This function verifies if the provided short key is valid among the parameters.
     *
     * @param short_key The short key to be checked.
     * @param params The container of parameters to be verified.
     * @return `true` if the short key is valid, otherwise `false`.
     */
    bool is_valid_single_key(char_type short_key,
        const container& params
    ) const {
        return is_valid_single_key_impl(short_key, params);
    }

    /**
     * @brief Checks if the given long key is valid and assigned in the provided parameters.
     *
     * This function verifies if the specified long key is valid and assigned in the given parameters.
     *
     * @param long_key The long key to be checked.
     * @param params The container of parameters to be verified.
     * @return `true` if the long key is valid and assigned, `false` otherwise.
     */
    bool is_valid_single_key(string_view_type long_key,
        const container& params
    ) const {
        return is_valid_single_key_impl(long_key, params);
    }

    /**
     * @brief Checks if a complex key is valid among parameters.
     *
     * This function verifies if the provided complex key is valid among the parameters.
     *
     * @param complex_key The complex key to be checked.
     * @param params The container of parameters to be verified.
     * @return `true` if the complex key is valid, otherwise `false`.
     */
    bool is_valid_complex_key(string_view_type complex_key,
        const container& params
    ) const {
        for (auto key : complex_key) {
            auto is_valid = false;

            auto is_boolean = [](const auto& p) {
                using value_type = typename
                    std::remove_cvref_t<decltype(p)>::value_type;

                return std::is_same_v<bool, value_type>
                    || std::is_base_of_v<bool, value_type>;
            };

            detail::tuple_for_each(params.data(),
                [&is_valid, key, is_boolean](const auto& p) {
                    if ( p.contains(key) && is_boolean(p) ) {
                        is_valid = true;
                    }
                }
            );

            if (!is_valid) {
                return false;
            }
        }

        return true;
    }

    /**
     * @brief Checks if all required keys are provided among parameters.
     *
     * This function verifies if all required keys are provided among the parameters.
     *
     * @param params The container of parameters to be verified.
     * @return `true` if all required keys are provided, otherwise `false`.
     */
    bool satisfies_required(const container& params) const {
        auto required_switches = bitset_type(0u);
        detail::tuple_for_each(params.data(),
            [&required_switches, i = 0](const auto& p) mutable {
                using none_qualified_t
                    = std::remove_cvref_t<decltype(p)>;
                using value_type = typename
                    none_qualified_t::value_type;
                using required_t = basic_required<
                    value_type, char_type, traits_type
                >;

                if constexpr ( std::is_base_of_v<
                    required_t, none_qualified_t
                >) {
                    if (!p.has_defval()) {
                        required_switches.set(i);
                    }
                }
                ++i;
            }
        );

        return (required_switches & switches_)
            == required_switches;
    }

    /**
     * @brief Sets a switch for the specified short key among parameters.
     *
     * This function sets a switch for the specified short key among the parameters, indicating it has been assigned.
     *
     * @param short_key The short key to set the switch for.
     * @param params The container of parameters to update.
     */
    void set_assigned(char_type short_key,
        const container& params
    ) {
        set_assigned_impl(short_key, params);
    }

    /**
     * @brief Marks the specified long key as assigned in the switches.
     *
     * This function marks the specified long key as assigned in the switches, indicating that the key has been used in the command line.
     *
     * @param long_key The long key to be marked as assigned.
     * @param params The container of parameters where the assignment occurs.
     */
    void set_assigned(string_view_type long_key,
        const container& params
    ) {
        set_assigned_impl(long_key, params);
    }

    /**
     * @brief Checks if a switch is set for the specified short key among parameters.
     *
     * This function checks if a switch is set for the specified short key among the parameters, indicating it has been assigned.
     *
     * @param short_key The short key to check the switch for.
     * @param params The container of parameters to check.
     * @return `true` if the switch is set, indicating the key is assigned, otherwise `false`.
     */
    bool assigned(char_type short_key,
        const container& params
    ) {
        return assigned_impl(short_key, params);
    }

    /**
     * @brief Checks if the specified long key is assigned in the switches.
     *
     * This function checks if the specified long key is assigned in the switches, indicating that the key has been used in the command line.
     *
     * @param long_key The long key to be checked for assignment.
     * @param params The container of parameters where the assignment occurs.
     * @return `true` if the long key is assigned, `false` otherwise.
     */
    bool assigned(string_view_type long_key,
        const container& params
    ) {
        return assigned_impl(long_key, params);
    }

    /**
     * @brief Marks the parsing process as a failure due to assignment of incompatible type.
     *
     * This function marks the parsing process as a failure due to assignment of incompatible types,
     * indicating that the provided command-line arguments have type mismatches with the associated parameters.
     *
     * @param fail If set to `true`, marks the parsing process as a failure due to type incompatibility, otherwise resets the fail status.
     */
    void mark_fail( bool fail = true ) {
        fail_ = fail;
    }

    /**
     * @brief Marks the parsing process as bad input due to argument lack or surplus.
     *
     * This function marks the parsing process as bad input due to arguments lacking or having surpluses,
     * indicating that the provided command-line arguments do not match the expected number of arguments for parameters.
     *
     * @param bad If set to `true`, marks the parsing process as bad input due to argument lack or surplus, otherwise resets the bad input status.
     */
    void mark_bad( bool bad = true ) {
        bad_ = bad;
    }

    /**
     * @brief Checks if the parsing process is in a good state.
     *
     * This function checks if the parsing process is in a good state, meaning neither bad bit nor fail bit is set.
     *
     * @return `true` if the parsing process is in a good state, otherwise `false`.
     */
    bool good() const noexcept {
        return !(bad_ || fail_);
    }

    /**
     * @brief Checks if the parsing failed due to arguments lacking or surplus.
     *
     * This function checks if the parsing failed due to the lack or surplus of arguments
     * (i.e., incorrect number of arguments provided for the corresponding keys).
     *
     * @return `true` if parsing failed due to arguments lack/surplus, `false` otherwise.
     */
    bool bad() const noexcept {
        return bad_;
    }

    /**
     * @brief Checks if the parsing process failed.
     *
     * This function checks if the parsing process failed, meaning the provided command-line arguments are not valid.
     *
     * @return `true` if the parsing process failed, otherwise `false`.
     */
    bool fail() const noexcept {
        return fail_;
    }

    /**
     * @brief Clears the internal states, resets switches, and marks the parsing process as successful.
     *
     * This function clears the internal states, resets switches for parameters, and marks the parsing process as successful.
     */
    void clear() {
        switches_.reset();
        mark_fail(false);
        mark_bad(false);
    }

private:
    /**
     * @brief Checks if the specified key has a duplicated assignment in the provided parameters.
     *
     * checks if the specified key has a duplicated assignment in the given parameters. It uses the switches bitset to track assigned parameters to prevent duplication.
     *
     * @tparam KeyT The type of the key (char_type or string_view_type).
     * @param key The key to be checked for duplicated assignment.
     * @param params The container of parameters to be checked.
     * @return `true` if the key has a duplicated assignment, `false` otherwise.
     */
    template <class KeyT>
    bool is_duplicated_assignment_impl(KeyT key,
        const container& params
    ) const {
        return is_duplicated_assignment_impl(
            key, params, this->switches_
        );
    }

    /**
     * @brief Checks if the specified key has a duplicated assignment in the provided parameters with custom switches.
     *
     * checks if the specified key has a duplicated assignment in the given parameters using custom switches. It prevents duplication based on the provided switches bitset.
     *
     * @tparam KeyT The type of the key (char_type or string_view_type).
     * @param key The key to be checked for duplicated assignment.
     * @param params The container of parameters to be checked.
     * @param switches The custom switches bitset to track assigned parameters.
     * @return `true` if the key has a duplicated assignment, `false` otherwise.
     */
    template <class KeyT>
    bool is_duplicated_assignment_impl(KeyT key,
        const container& params, const bitset_type& switches
    ) const {
        bool is = false;

        detail::tuple_for_each(
            params.data(),
            [&is, switches, key, i = 0](const auto& p) mutable {
                if (p.contains(key)) {
                    is = switches.test(i);
                }

                ++i;
            }
        );

        return is;
    }

    /**
     * @brief Checks if the specified single key is valid in the provided parameters.
     *
     * checks if the specified single key is valid in the given parameters.
     *
     * @tparam KeyT The type of the key (char_type or string_view_type).
     * @param key The single key to be checked for validity.
     * @param params The container of parameters to be checked.
     * @return `true` if the single key is valid, `false` otherwise.
     */
    template <class KeyT>
    bool is_valid_single_key_impl(KeyT key,
        const container& params
    ) const {
        bool is = false;

        detail::tuple_for_each(params.data(),
            [&is, key](const auto& p) {
                if (p.contains(key)) {
                    is = true;
                }
            }
        );

        return is;
    }

    /**
     * @brief Sets the switch corresponding to the specified key in the switches bitset.
     *
     * sets the switch corresponding to the specified key in the switches bitset.
     *
     * @tparam KeyT The type of the key (char_type or string_view_type).
     * @param key The key for which the switch needs to be set.
     * @param params The container of parameters containing the key.
     */
    template <class KeyT>
    void set_assigned_impl(KeyT key, const container& params) {
        return set_assigned_impl(key, params, this->switches_);
    }

    /**
     * @brief Sets the switch corresponding to the specified key in the custom switches bitset.
     *
     * sets the switch corresponding to the specified key in the custom switches bitset.
     *
     * @tparam KeyT The type of the key (char_type or string_view_type).
     * @param key The key for which the switch needs to be set.
     * @param params The container of parameters containing the key.
     * @param switches The custom switches bitset where the switch will be set.
     */
    template <class KeyT>
    void set_assigned_impl(KeyT key, const container& params,
        bitset_type& switches
    ) const {
        detail::tuple_for_each(
            params.data(),
            [&switches, key, i = 0](const auto& p) mutable {
                if (p.contains(key)) {
                    switches.set(i);
                }
                ++i;
            }
        );
    }

    /**
     * @brief Checks if the specified key is assigned in the provided parameters.
     *
     * checks if the specified key is assigned in the given parameters. It uses the switches bitset to determine assignment status.
     *
     * @tparam KeyT The type of the key (char_type or string_view_type).
     * @param key The key to be checked for assignment.
     * @param params The container of parameters to be checked.
     * @return `true` if the key is assigned, `false` otherwise.
     */
    template <class KeyT>
    bool assigned_impl(KeyT key,
        const container& params
    ) {
        bool ass = false;
        
        detail::tuple_for_each(
            params.data(),
            [&ass, this, key, i = 0](const auto& p) mutable {
                ass = p.contains(key) &&
                    this->switches_.test(i);

                ++i;
            }
        );

        return ass;
    }

    string_view_type identifier_;
    bitset_type switches_;
    bool fail_;
    bool bad_;
};  // class basic_cl_parser::verifier

/**
 * @brief The `assigner` class handles the assignment of command-line arguments to corresponding parameters.
 * 
 * The `assigner` class is responsible for processing complex keys, short keys, and long keys along with their associated arguments.
 * It assigns values to parameters based on the provided keys and arguments. If an assignment fails due to type incompatibility
 * or lack/surplus of arguments, the `assigner` marks the associated verifier's fail or bad flag, indicating the cause of the failure.
 * Unassigned arguments should be retrieved using the `get_unassigned()` function, or cleaned using `clear()` after a failure.
 */
class assigner {
public:
    /**
     * @brief Constructs an `assigner` object with the specified container of parameters.
     * 
     * @param params The container of parameters to which the command-line arguments will be assigned.
     */
    assigner(container& params)
        : params_(params), stream_() {}

    /**
     * @brief Assigns values based on a complex key to the associated parameters.
     * 
     * This function processes a complex key, assigns values to the corresponding parameters,
     * and updates the verifier's fail and bad flags accordingly.
     * 
     * @param complex_key The complex key to be processed (e.g., "-abc").
     * @param veri The verifier object associated with the assignment operation.
     */
    void assign_complex(
        string_view_type complex_key, verifier& veri
    ) {
        /*
        fail means at least one of the keys in complex param received
        isn't defined as boolean param.
        */ 
        for (auto key : complex_key) {
            auto key_found = false;

            detail::tuple_for_each(
                params_.data(),
                [ &key_found, &veri, key ](auto& p) {
                    if (p.contains(key)) {
                        key_found = true;
                        p.assign(true);
                        veri.mark_fail( p.fail() );
                    }
                }
            );

            if (!key_found) {
                veri.mark_fail();
            }
            else {
                veri.set_assigned(key, params_);
            }
        }

        if ( veri.good() ) {
            params_.invalidate_cache();
        }
        else {
            // do not override error source.
            // (if fail, has_unassigned() is always true
            // but it's just the consequence of failure)
            veri.mark_bad( has_unassigned() );
        }
    }

    /**
     * @brief Assigns a value based on a short key and arguments to the associated parameters.
     * 
     * This function processes a short key and its associated arguments, assigns the value to the
     * corresponding parameter, and updates the verifier's fail and bad flags accordingly.
     * 
     * @param short_key The short key for which the assignment is made.
     * @param args The arguments associated with the short key.
     * @param veri The verifier object associated with the assignment operation.
     */
    void assign_single(char_type short_key,
        const words_type& args, verifier& veri
    ) {
        assign_by_idx( getidx(short_key), args, veri );
        veri.set_assigned(short_key, params_);

        if ( veri.good() ) {
            params_.invalidate_cache();
        }
        else {
            // do not override error source.
            // (if fail, has_unassigned() is always true
            // but it's just the consequence of failure)
            veri.mark_bad( has_unassigned() );
        }
    }

    /**
     * @brief Assigns a value based on a long key and arguments to the associated parameters.
     * 
     * This function processes a long key and its associated arguments, assigns the value to the
     * corresponding parameter, and updates the verifier's fail and bad flags accordingly.
     * 
     * @param long_key The long key for which the assignment is made.
     * @param args The arguments associated with the long key.
     * @param veri The verifier object associated with the assignment operation.
     */
    void assign_single(string_view_type long_key,
        const words_type& args, verifier& veri
    ) {
        assign_by_idx( getidx(long_key), args, veri );
        veri.set_assigned(long_key, params_);

        if ( veri.good() ) {
            params_.invalidate_cache();
        }
        else {
            // do not override error source.
            // (if fail, has_unassigned() is always true
            // but it's just the consequence of failure)
            veri.mark_bad( has_unassigned() );
        }
    }

    /**
     * @brief Checks if there are unassigned arguments in the internal stream.
     * 
     * @return `true` if there are unassigned arguments; `false` otherwise.
     */
    bool has_unassigned() const {
        // if any character rather than eof remain in stream,
        // stream buffer size is greater than 1. (the one stands for eof.)
        return stream_.rdbuf()->in_avail() > 1ull;
    }

    /**
     * @brief Retrieves unassigned arguments from the internal stream.
     * 
     * This function retrieves unassigned arguments from the internal stream and clears the stream.
     * 
     * @return Unassigned arguments as a string.
     */
    string_type get_unassigned() {
        auto args = string_type();

        auto view = std::ranges::basic_istream_view<
            string_type, char_type, traits_type
        >( stream_ );

        for (const auto& s : view) {
            std::format_to( std::back_inserter(args),
                __LITERAL(char_type, "\"{}\" "),
                s
            );
        }

        if (!args.empty()) {
            args.pop_back();
        }

        clear();

        return args;
    }

    /**
     * @brief Clears the internal input stream.
     * 
     * This function clears the internal input stream, discarding any remaining characters.
     */
    void clear() {
        stream_.clear();
        if (stream_.rdbuf()->in_avail() > 1ull) {
            stream_.ignore(
                std::numeric_limits<std::streamsize>::max()
            );
        }
    }

private:
    /**
     * @brief Assigns values to parameters based on the specified index and arguments.
     * 
     * This function assigns values to parameters based on the specified index and arguments.
     * It updates the verifier's fail and bad flags accordingly.
     * 
     * @param idx The index of the parameter to which the assignment is made.
     * @param args The arguments associated with the parameter.
     * @param veri The verifier object associated with the assignment operation.
     */
    void assign_by_idx(index_type idx,
        const words_type& args, verifier& veri
    ) {
        detail::tuple_for_each(
            params_.data(),
            [&args, &veri, idx, this, i = 0](auto& p) mutable {
                using value_type = typename std::remove_cvref_t<decltype(p)>
                    ::value_type;

                if (idx == i) {
                    // when extracting string_type,
                    // all characters in the stream regardless of white space should be extracted.
                    if constexpr (std::is_base_of_v<string_type, value_type>) {
                        auto acc = string_type();
                        for (auto&& s : std::views::istream<string_type>(stream_args(args))) {
                            acc += s;
                            acc += detail::stream_delim<char_type>();
                        }
                        acc.pop_back();
                        p.assign( std::move(acc) );
                        stream_.clear();
                    }
                    // when extracting boolean type,
                    // omitting argument means the value is true
                    else if constexpr (std::is_base_of_v<bool, value_type>
                        || std::is_same_v<bool, value_type>
                    ) {
                        if (std::empty(args)) {
                            p.assign(true);
                        }
                        else {
                            // try boolalpha and integral
                            stream_args(args);
                            stream_ >> std::boolalpha >> p;
                            if (p.fail()) {
                                p.clear();
                                stream_ >> std::noboolalpha >> p;
                            }
                            stream_ >> std::noboolalpha;
                        }
                    }
                    else {
                        stream_args(args) >> p;
                    }

                    veri.mark_fail( p.fail() );
                }
                ++i;
            }
        );
    }

    /**
     * @brief Retrieves the index of the parameter associated with the specified short key.
     * 
     * @param short_key The short key for which the index needs to be retrieved.
     * @return The index of the parameter associated with the short key.
     */
    index_type getidx(char_type short_key) const {
        return getidx_impl(short_key);
    }

    /**
     * @brief Retrieves the index of the parameter associated with the specified long key.
     * 
     * @param long_key The long key for which the index needs to be retrieved.
     * @return The index of the parameter associated with the long key.
     */
    index_type getidx(string_view_type long_key) const {
        return getidx_impl(long_key);
    }

    template <class KeyT>
    index_type getidx_impl(KeyT key) const {
        // assume the key's validity is already checked.
        auto ret = index_type(0);

        detail::tuple_for_each(
            params_.data(),
            [&ret, key, i = index_type(0)](const auto& p) mutable {
                if (p.contains(key)) {
                    ret = i;
                }
                ++i;
            }
        );

        return ret;
    }

    /**
     * @brief Writes arguments to the internal stream for processing.
     * 
     * This function writes arguments to the internal stream for processing.
     * 
     * @param args The arguments to be written to the stream.
     * @return A reference to the internal input stream after writing the arguments.
     */
    stream_type& stream_args(const words_type& args) {
        std::ranges::for_each(args, [this](const auto& arg) {
            stream_ << arg << detail::stream_delim<char_type>();
        });

        return stream_;
    }

    container& params_; /**< Reference to the container of parameters. */
    stream_type stream_; /**< Internal input stream used for processing arguments. */
};  // class basic_cl_parser::assigner

/**
 * @brief The `logger` class provides error logging functionality for the command-line parser.
 * 
 * The `logger` class is responsible for logging various error messages related to command-line parsing.
 * It includes functions to log specific error scenarios, such as incompatible arguments, missing keys,
 * unparsed arguments, and duplicated assignments. The error messages are streamed to an internal error
 * stream, and the error code associated with the first encountered error is set and locked.
 * The difference between behavior of error code and error source arises from the fact that
 * both tracking the exact error source(the first error encountered) and providing all information of
 * other possible error sources are important.
 * 
 * After the error handled, the internal state of `logger` object should be cleared
 * using `clear()` after processing the error.
 */
class logger {
public:
    /**
     * @brief Constructs a logger object associated with the specified parameter container.
     * 
     * @param params The parameter container used for error reporting and validation.
     */
    logger(const container& params)
        : err_stream_(), params_(params),
        err_code_() {}

    /**
     * @brief Logs an error when the identifier is not given in the command-line input.
     */
    void log_error_identifier_not_given() {
        lock_error(error_code::identifier_not_given);
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: didn't receive identifier, command-line is empty.\n"
        );
    }

    /**
     * @brief Logs an error when incompatible arguments are provided for a specific key.
     * 
     * @param key The key for which incompatible arguments are provided.
     * @param args_string The string representation of the incompatible arguments.
     */
    template <class KeyT>
    void log_error_incompatible_arguments(
        KeyT key, string_view_type args_string
    ) {
        lock_error(error_code::incompatible_argument);
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: received arguments are incompatible with"
            " the specified key \""
        ) << key << __LITERAL(char_type,
            "\".\n"
            "\treceived: ["
        ) << args_string << __LITERAL(char_type,
            "]\n";
        );
    }

    /**
     * @brief Logs an error when an invalid identifier is specified.
     * 
     * @param received_identifier The received invalid identifier.
     * @param correct_identifier The correct expected identifier.
     */
    void log_error_invalid_identifier(
        string_view_type received_identifier,
        string_view_type correct_identifier
    ) {
        lock_error(error_code::invalid_identifier);
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: invalid identifier specified.\n"
            "\texpected \"*\\\\"
        ) << correct_identifier << __LITERAL(char_type,
            "\" but received \""
        ) << received_identifier << __LITERAL(char_type,
            "\"\n"
        );
    }

    /**
     * @brief Logs an error when a key is not given in the command-line input.
     */
    void log_error_key_not_given() {
        lock_error(error_code::key_not_given);
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: key is not given.\n"
        );
    }

    /**
     * @brief Logs an error when an undefined key is received in the command-line input.
     * 
     * @param key The undefined key received.
     */
    template <class KeyT>
    void log_error_undefined_key(KeyT key) {
        lock_error(error_code::undefined_key);
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: undefined key \""
        ) << key << __LITERAL(char_type,
            "\" received.\n"
        );
    }

    /**
     * @brief Logs an error when unparsed arguments are detected in the command-line input.
     * 
     * @param args_string The string representation of unparsed arguments.
     */
    void log_error_unparsed_arguments(
        string_view_type args_string
    ) {
        lock_error(error_code::unparsed_argument);
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: unparsed arguments detected.\n"
            "\tremaining tokens: "
        );
        err_stream_ << args_string;
        err_stream_ << __LITERAL(char_type,
            "\n"
        );
    }

    /**
     * @brief Logs an error when a wrong complex key is received.
     * 
     * If at least one of the keys in complex param received isn't defined as boolean param
     * or at least one key in a complex param duplicated, it's wrong complex key error.
     * @param keys The wrong complex key received.
     */
    template <class KeyT>
    void log_error_wrong_complex_key(KeyT keys) {
        lock_error(error_code::incompatible_argument);
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: at least one of the keys in complex param received"
            " isn't defined as boolean param"
            " or at least one key in a complex param duplicated.\n"
            "\treceived: \""
        ) << keys << __LITERAL(char_type,
            "\"\n"
        );
    }

    /**
     * @brief Logs an error when required keys are not given in the command-line input.
     */
    void log_error_required_key_not_given() {
        lock_error(error_code::required_key_not_given);
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: required keys are not given.\n"
            "required keys:\n"
        );

        auto print_keys_to_err = [this](auto&& keys) {
            std::ranges::for_each(
                std::begin(keys), std::prev(std::end(keys)),
                [this](const auto& key) {
                    err_stream_ << key << __LITERAL(char_type, "|");
                }
            );

            err_stream_ << *std::prev(std::end(keys));
        };

        detail::tuple_for_each(params_.data(),
            [print_keys_to_err, this](const auto& p) {
                using none_qualified_t
                    = std::remove_cvref_t<decltype(p)>;
                using value_type = typename none_qualified_t::value_type;

                if constexpr ( std::is_base_of_v<
                        basic_required<value_type, char_type, traits_type>,
                        none_qualified_t
                    >
                ) {
                    err_stream_ << __LITERAL(char_type, "\t[");
                    print_keys_to_err(p.short_keys());
                    err_stream_ << __LITERAL(char_type, "|");
                    print_keys_to_err(p.long_keys());
                    err_stream_ << __LITERAL(char_type, "]: ")
                        << p.brief_message() << __LITERAL(char_type, "\n");
                }
            }
        );
    }

    /**
     * @brief Logs an error when duplicated assignments are detected for a specific key.
     * 
     * @param key The key for which duplicated assignments are detected.
     */
    template <class KeyT>
    void log_error_duplicated_assignments(KeyT key) {
        lock_error(error_code::duplicated_assignments);
        err_stream_ << __LITERAL(char_type,
            "[gclp] error: duplicated assignments detected when parsing \""
        ) << key << __LITERAL(char_type,
            "\".\n"
            "\tmore than one of keys are assigning their values to same parameter.\n"
        );
    }

    /**
     * @brief Retrieves the error code associated with the first encountered error.
     * 
     * @return The error code, if an error occurred; otherwise, std::nullopt.
     */
    std::optional<error_code> error() const noexcept {
        return err_code_;
    }

    /**
     * @brief Retrieves the full error message string containing all encountered errors.
     * 
     * @return The full error message string. If no errors have occurred, an empty string is returned.
     */
    string_view_type error_message() const noexcept {
        return err_stream_.rdbuf()->view();
    }

    /**
     * @brief Clears the internal state of the logger, including error code and error message stream.
     */
    void clear() {
        err_code_.reset();
        err_stream_.clear();
        if (err_stream_.rdbuf()->in_avail() > 1ull) {
            err_stream_.ignore(
                std::numeric_limits<std::streamsize>::max()
            );
        }
    }
private:
    /**
     * @brief Locks the error code if it is not already set.
     * 
     * @param ec The error code to be set if it is not already set.
     */
    void lock_error(error_code ec) {
        if ( !error() ) {
            err_code_ = ec;
        }
    }

    stream_type err_stream_; /**< Internal error message stream for logging error messages. */
    const container& params_; /**< Reference to the container of parameters used for validation and reporting. */
    std::optional<error_code> err_code_; /**< Optional error code associated with the first encountered error. */
};  // class basic_cl_parser::logger

public:
    /**
     * @brief Constructs a basic_cl_parser object, specifying the expected command-line identifier and parameters to be parsed.
     * 
     * @param identifier The expected command-line identifier (e.g., program name).
     * @param params Parameter objects representing the options and arguments to be parsed.
     */
    basic_cl_parser(string_view_type identifier, Params... params)
        : cont_( std::move(params)... ), veri_(identifier),
        assi_(cont_), logg_(cont_) {}

    /**
     * @brief Gets the error code if any parsing errors occurred.
     * 
     * @return An optional error code if errors occurred during parsing, otherwise std::nullopt.
     */
    std::optional<error_code> error() const noexcept {
        return logg_.error();
    }

    /**
     * @brief Gets the full error message containing all encountered errors during parsing.
     * 
     * @return The full error message string. If no errors have occurred, an empty string is returned.
     */
    string_view_type error_message() const noexcept {
        return logg_.error_message();
    }

    /**
     * @brief Parses command-line arguments from an array of strings.
     * 
     * @tparam IntType The type of the argument count.
     * @tparam StrArrType The type of the argument array.
     * @param argc The number of command-line arguments.
     * @param argv An array of command-line argument strings.
     * @return A reference to a tuple containing parsed parameter values.
     * 
     * @details This function processes the command-line arguments passed as an array of strings, 
     * interprets the options and values, and returns a tuple containing the parsed parameter values.
     * If errors occur during parsing, they are recorded internally and can be retrieved using the error() and error_message() functions.
     * 
     * @note The function modifies the internal state of the parser and should not be called again after errors occur unless the internal state is reset.
     */
    template <class IntType, class StrArrType>
    result_tuple_type& parse(IntType argc, StrArrType argv) {
        // if error is not cleared, return early.
        if ( logg_.error() ) {
            return get();
        }

        return parse_impl( interpreter(argc, argv) );
    }

    /**
     * @brief Parses command-line arguments from a single string.
     * 
     * @param command_line The command-line string to parse.
     * @return A reference to a tuple containing parsed parameter values.
     * 
     * @details This function processes the command-line arguments passed as a single string, 
     * interprets the options and values, and returns a tuple containing the parsed parameter values.
     * If errors occur during parsing, they are recorded internally and can be retrieved using the error() and error_message() functions.
     * 
     * @note The function modifies the internal state of the parser and should not be called again after errors occur unless the internal state is reset.
     */
    result_tuple_type& parse(string_view_type command_line) {
        // if error is not cleared, return early.
        if ( logg_.error() ) {
            return get();
        }

        return parse_impl( interpreter(command_line) );
    }

    /**
     * @brief Gets the parsed parameter values.
     * 
     * @return A reference to a tuple containing parsed parameter values.
     * 
     * @details This function returns a reference to the tuple containing the parsed parameter values.
     * The user can access the individual parameters' values from the returned tuple.
     */
    result_tuple_type& get() {
        return cont_.values();
    }

    /**
     * @brief Gets the parsed parameter values (const version).
     * 
     * @return A const reference to a tuple containing parsed parameter values.
     * 
     * @details This function returns a const reference to the tuple containing the parsed parameter values.
     * The user can access the individual parameters' values from the returned tuple.
     */
    const result_tuple_type& get() const {
        return cont_.values();
    }

    /**
     * @brief Initializes the internal state of the parser, clearing any previous parsing results and errors.
     * 
     * @details This function resets the internal state of the parser, clearing any previous parsing results and errors.
     * It prepares the parser for a fresh parsing operation.
     */
    void clear() {
        cont_.clear();
        veri_.clear();
        logg_.clear();
    }

private:
    /**
     * @brief Implementation of the parsing process using an interpreter.
     * 
     * @param ip An interpreter for tokenizing the command-line.
     * @return A reference to a tuple containing parsed parameter values.
     * 
     * @details This function is an internal implementation used by the public parse functions.
     * It takes an interpreter object, which tokenizes the command-line and processes the options and arguments.
     * The function interprets the tokens, validates the parameters, and records any errors encountered during parsing.
     * If errors occur, they are logged internally and can be retrieved using the error() and error_message() functions.
     * 
     * @note The function modifies the internal state of the parser and should not be called directly by users.
     */
    result_tuple_type& parse_impl(interpreter&& ip) {
        clear();

        if (ip.done()) {
            logg_.log_error_identifier_not_given();
        }
        
        auto [id, arg_unexpected] = ip.get_token();

        if ( !veri_.is_valid_identifier(id) ) {
            logg_.log_error_invalid_identifier(
                id, veri_.id()
            );
        }

        if ( !std::ranges::empty(arg_unexpected) ) {
            logg_.log_error_key_not_given();
        }

        while (!ip.done()) {
            if ( !ip.facing_key() ) {
                logg_.log_error_key_not_given();
            }

            auto tok = ip.get_token();

            if ( veri_.starts_with_short_key(tok) ) {
                auto key = ip.remove_dash(tok.leading);
                const auto& args = tok.followings;
                // token's leading factor is string_view_type,
                // for represent short key that is char_type,
                // fetch the front element.
                parse_single_key( *std::begin(key), args );
            }
            else if ( veri_.starts_with_long_key(tok) ) {
                auto key = ip.remove_dash(tok.leading);
                const auto& args = tok.followings;
                parse_single_key(key, args);
            }
            else if ( veri_.starts_with_complex_key(tok) ) {
                auto key = ip.remove_dash(tok.leading);
                parse_complex_key(key);
            }
            else {
                // this branch cannot be reached.
                // if reached, it's pragmma fault.
                assert(false);
            }
        }

        if ( !veri_.satisfies_required(cont_) ) {
            logg_.log_error_required_key_not_given();
        }

        if (!logg_.error()) {
            cont_.update_cache();
        }

        return get();
    }

    /**
     * @brief Parses a single key-value pair from the command-line arguments.
     * 
     * @tparam KeyT The type of the key (character or string).
     * @tparam Args The type of the arguments associated with the key.
     * @param key The key to be parsed (either short or long key).
     * @param args The arguments associated with the key.
     * 
     * @details This function parses a single key and its associated arguments from the command-line input.
     * It validates the key's existence and uniqueness, assigns the provided arguments to the corresponding parameter,
     * and checks for compatibility issues (such as type mismatch). If errors occur during parsing, appropriate error
     * messages are logged for further reference. The parsing results are stored internally in the parser's state.
     */
    template <class KeyT, class Args>
    void parse_single_key(KeyT key, Args&& args) {
        if ( !veri_.is_valid_single_key(key, cont_) ) {
            logg_.log_error_undefined_key(key);
        }
        if ( veri_.is_duplicated_assignment(key, cont_) ) {
            logg_.log_error_duplicated_assignments(key);
        }

        assi_.assign_single(key, std::forward<Args>(args), veri_);

        if (veri_.fail()) {
            logg_.log_error_incompatible_arguments(
                key, assi_.get_unassigned()
            );
        }
        if (veri_.bad()) {
            logg_.log_error_unparsed_arguments(
                assi_.get_unassigned()
            );
        }
    }

    /**
     * @brief Parses a complex key (combination of multiple keys) from the command-line arguments.
     * 
     * @param keys The complex key represented as a string_view_type.
     * 
     * @details This function parses a complex key, which is a combination of multiple individual keys, from the command-line input.
     * It validates the existence and uniqueness of each key within the complex key, assigns the appropriate boolean values to the
     * corresponding parameters, and ensures that no assignment conflicts occur. If errors are detected, appropriate error messages
     * are logged. Complex keys do not have associated arguments, so they are tokenized without arguments. Any parsing issues
     * detected during complex key parsing are flagged as errors. The parser's state is updated accordingly.
     * 
     * @note If the verifier's bad bit is set after parsing a complex key, it indicates a programming error since complex keys
     * should not have associated arguments. An assertion failure occurs in such cases.
     */
    void parse_complex_key(string_view_type keys) {
        if ( !veri_.is_valid_complex_key(keys, cont_) ) {
            logg_.log_error_wrong_complex_key(keys);
        }
        if ( veri_.is_duplicated_complex_assignment(keys, cont_) ) {
            logg_.log_error_duplicated_assignments(keys);
        }

        assi_.assign_complex(keys, veri_);

        if (veri_.fail()) {
            logg_.log_error_wrong_complex_key(keys);
        }
        
        // verifier's bad bit connot be set,
        // complex key is tokenized without arguments.
        // so if bad bit is set, it's pragmma fault.
        assert( !veri_.bad() );
    }

    container cont_;
    verifier veri_;
    assigner assi_;
    logger logg_;
};  // class basic_cl_parser

/**
 * @brief Shortcut type alias for optional command-line parameter with char character type.
 * 
 * Represents an optional command-line parameter with char character type.
 * @tparam ValT The value type of the parameter.
 */
template <class ValT>
using optional = basic_optional<ValT, char, std::char_traits<char>>;

/**
 * @brief Shortcut type alias for optional command-line parameter with wchar_t character type.
 * 
 * Represents an optional command-line parameter with wchar_t character type.
 * @tparam ValT The value type of the parameter.
 */
template <class ValT>
using woptional = basic_optional<ValT, wchar_t, std::char_traits<wchar_t>>;

/**
 * @brief Shortcut type alias for required command-line parameter with char character type.
 * 
 * Represents a required command-line parameter with char character type.
 * @tparam ValT The value type of the parameter.
 */
template <class ValT>
using required = basic_required<ValT, char, std::char_traits<char>>;

/**
 * @brief Shortcut type alias for required command-line parameter with wchar_t character type.
 * 
 * Represents a required command-line parameter with wchar_t character type.
 * @tparam ValT The value type of the parameter.
 */
template <class ValT>
using wrequired = basic_required<ValT, wchar_t, std::char_traits<wchar_t>>;

/**
 * @brief Shortcut type alias for command-line parser with char character type.
 * 
 * Represents a command-line parser with char character type.
 * @tparam Params Parameter types to be parsed by the parser.
 */
template <class ... Params>
using parser = basic_cl_parser<char, std::char_traits<char>, Params...>;

/**
 * @brief Shortcut type alias for command-line parser with wchar_t character type.
 * 
 * Represents a command-line parser with wchar_t character type.
 * @tparam Params Parameter types to be parsed by the parser.
 */
template <class ... Params>
using wparser = basic_cl_parser<wchar_t, std::char_traits<wchar_t>, Params...>;

}   // namespace gclp

#endif  // __GCLP