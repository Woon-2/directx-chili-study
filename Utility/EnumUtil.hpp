#ifndef __EnumUtil
#define __EnumUtil

#include <type_traits>

#define DEFINE_ENUM_LOGICAL_OP(Enum_t, int_t) \
    DEFINE_ENUM_INT_OP(Enum_t, int_t, &, [](auto lhs, auto rhs) { return lhs & rhs; })  \
    DEFINE_ENUM_INT_OP(Enum_t, int_t, |, [](auto lhs, auto rhs) { return lhs | rhs; })  \
    DEFINE_ENUM_INT_OP(Enum_t, int_t, &&, [](auto lhs, auto rhs) { return lhs && rhs; })    \
    DEFINE_ENUM_INT_OP(Enum_t, int_t, ||, [](auto lhs, auto rhs) { return lhs || rhs; })

#define DEFINE_ENUM_INT_OP(Enum_t, int_t, opSymbol, opIntFunc)   \
    DEFINE_BINARY_OP(Enum_t, int_t, opSymbol,   \
        [](auto e, auto i) { return opIntFunc( EnumUtil::etoi(e), i ); } \
    )   \
    DEFINE_BINARY_OP(int_t, Enum_t, opSymbol,    \
        [](auto i, auto e) { return opIntFunc( i, EnumUtil::etoi(e) ); } \
    )   \
    DEFINE_BINARY_OP(Enum_t, Enum_t, opSymbol,  \
        [](auto e1, auto e2) { return opIntFunc( EnumUtil::etoi(e1), EnumUtil::etoi(e2) ); } \
    ) 

#define DEFINE_BINARY_OP(Lhs_t, Rhs_t, opSymbol, opFunc)    \
    decltype(auto) operator opSymbol (Lhs_t lhs, Rhs_t rhs)   \
        noexcept( noexcept(opFunc(lhs, rhs)) )  \
    {   \
        return opFunc( std::forward<Lhs_t>(lhs),    \
            std::forward<Rhs_t>(rhs) ); \
    }

namespace EnumUtil
{
    template <class Enum>
    constexpr decltype(auto) etoi(Enum e)
    {
        return static_cast< std::underlying_type_t<Enum> >(e);
    }

}

#endif  // __EnumUtil