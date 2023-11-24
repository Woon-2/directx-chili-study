#ifndef __EnumUtil
#define __EnumUtil

#include <utility>
#include <type_traits>

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

template <class EnumT>
constexpr auto etoi(EnumT e) noexcept
    -> std::underlying_type_t<EnumT> {
    return static_cast<std::underlying_type_t<EnumT>>(e);
}

#endif  // EnumUtil