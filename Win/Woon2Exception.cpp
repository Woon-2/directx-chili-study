#include "Woon2Exception.hpp"

#include <sstream>

template <class CharT, class Traits, class Allocator>
Woon2Exception<CharT, Traits, Allocator>::
Woon2Exception( int lineNum, const CharT* fileStr ) noexcept
    : lineNum_(lineNum), fileStr_(fileStr)
{}

template <class CharT, class Traits, class Allocator>
const CharT* Woon2Exception<CharT, Traits, Allocator>::
what() const noexcept
{
    auto oss = std::basic_ostringstream<CharT, Traits, Allocator>();
    oss << getType() << '\n'
        << getMetaStr() << '\n';
    whatBuffer_ = oss.str();
    return whatBuffer_.c_str();
}

template <class CharT, class Traits, class Allocator>
const CharT* Woon2Exception<CharT, Traits, Allocator>::
getType() const noexcept
{
    if constexpr( std::is_same_v<CharT, char> ) {
        return "Woon2 Exception";
    }
    else if constexpr( std::is_same_v<CharT, wchar_t> ) {
        return L"Woon2 Exception";
    }
    else {
        static_assert("There's no matching character type on Woon2Exception to CharT.");
        return nullptr;
    }
}

template <class CharT, class Traits, class Allocator>
int Woon2Exception<CharT, Traits, Allocator>::
getLineNum() const noexcept
{
    return lineNum_;
}

template <class CharT, class Traits, class Allocator>
const typename Woon2Exception<CharT, Traits, Allocator>::String&
Woon2Exception<CharT, Traits, Allocator>::getFileStr() const noexcept
{
    return fileStr_;
}

template <class CharT, class Traits, class Allocator>
typename Woon2Exception<CharT, Traits, Allocator>::String
Woon2Exception<CharT, Traits, Allocator>::getMetaStr() const noexcept
{
    auto oss = std::basic_ostringstream<CharT, Traits, Allocator>();
    
    if constexpr( std::is_same_v<CharT, char> ) {
        oss << "[File] " << fileStr_ << '\n'
            << "[Line] " << lineNum_ << '\n';
        return oss.str();
    }
    else if constexpr( std::is_same_v<CharT, wchar_t> ) {
        oss << L"[File] " << fileStr_ << '\n'
            << L"[Line] " << lineNum_ << '\n';
        return oss.str();
    }
    else {
        static_assert("There's no matching character type on Woon2Exception to CharT.");
        return String();
    }
}