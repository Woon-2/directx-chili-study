#include <iostream>
#include "EnumUtil.hpp"

enum class Class {
    Math = 0x01, English = 0x02, Physics = 0x04, Chemistry = 0x08, Philosophy = 0x10, History = 0x20, Economics = 0x40, Biology  = 0x80 
};

DEFINE_ENUM_LOGICAL_OP(Class, std::size_t)

int main()
{
    std::cout << "Math | English = " << (Class::Math | Class::English) << '\n';
    std::cout << "Math | English | Physics | Chemistry = " << (Class::Math | Class::English | Class::Physics | Class::Chemistry) << '\n';
    std::cout << "Math && Math = " << std::boolalpha << (Class::Math && Class::Math) << '\n';
    std::cout << "Math || English = " << std::boolalpha << (Class::Math || Class::English) << '\n';

    std::cin.get();
}