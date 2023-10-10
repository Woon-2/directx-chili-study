#ifndef __Literal
#define __Literal

#include <type_traits>

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

#endif  // __Literal