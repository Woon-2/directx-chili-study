#ifndef __StringLike
#define __StringLike

#include "AdditionalConcepts.hpp"

#include <type_traits>
#include <string>
#include <string_view>

template <class>
struct is_char : std::false_type
{};

template <class T>
requires contains<T, char, wchar_t, char8_t, char16_t, char32_t,
    signed char, unsigned char>
struct is_char<T> : std::true_type
{};
    
template <class T>
inline constexpr auto is_char_v = is_char<T>::value;

template <class T>
struct is_string_like : std::false_type
{};

template <class T>
requires is_char_v< std::remove_cv_t< std::remove_pointer_t< std::decay_t<T> > > >
    && std::is_compound_v<T>
struct is_string_like<T> : std::true_type
{};

template < template<class ...> class T, class CharT, class ... Us>
requires
    requires {
        typename T<char>::value_type;
        typename T<char>::traits_type;
    }
    && ( std::is_convertible_v< T<char>, std::basic_string_view<char> >
        || std::is_convertible_v< T<char>, std::basic_string<char> >
    )
    && is_char_v<CharT>
struct is_string_like<T<CharT, Us...>> : std::true_type
{};

template <class T>
requires
    requires {
        typename T::value_type;
        typename T::traits_type;
    }
    && (    std::is_convertible_v<
            T,
            std::basic_string_view<
                typename T::value_type,
                typename T::traits_type
                >
            >
        ||  std::is_convertible_v<
            T,
            std::basic_string<
                typename T::value_type,
                typename T::traits_type
                >
            >
        )
    && is_char_v< typename T::value_type >
struct is_string_like<T> : std::true_type
{};

template <class T>
inline constexpr auto is_string_like_v = is_string_like< T >::value;

template <class T>
concept StringLike = is_string_like_v< std::remove_cvref_t<T> >;

template <StringLike S>
struct string_traits
{
    using type = std::char_traits<
        std::remove_cv_t<
            std::remove_pointer_t<
                std::decay_t<
                    std::remove_cvref_t<S>
                >
            >
        >
    >;
};

template <StringLike S>
requires requires { typename std::remove_cvref_t<S>::traits_type; }
struct string_traits<S>
{
    using type = typename std::remove_cvref_t<S>::traits_type;
};

template <StringLike S>
using string_traits_t = typename string_traits<S>::type;

template <StringLike ... Ss>
struct common_string_traits
{
    using type = std::common_type_t< string_traits_t<Ss>... >;
};

template <StringLike ... Ss>
using common_string_traits_t = typename common_string_traits<Ss...>::type;

#endif  // __StringLike