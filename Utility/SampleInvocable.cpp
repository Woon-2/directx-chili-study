#include "Invocable.hpp"

template <class T>
consteval bool bar(T var)
{
    if constexpr( is_invocation_valid( [](auto v) -> decltype( v.foo() ){}, var ) )
       return true;
    return false;
}

struct A
{
    void foo() {}
};

struct B
{};

int main() {
    static_assert(bar(A{}));
    static_assert(!bar(B{}));
}