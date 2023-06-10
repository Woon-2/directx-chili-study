#include "Woon2Exception.hpp"

#include <sstream>

Woon2Exception::Woon2Exception( int lineNum,
    const char* fileStr ) noexcept
    : lineNum_(lineNum), fileStr_(fileStr)
{}


const char* Woon2Exception::what() const noexcept
{
    auto oss = std::ostringstream();
    oss << getType() << '\n'
        << getMetaStr() << '\n';
    whatBuffer_ = oss.str();
    return whatBuffer_.c_str();
}

const char* Woon2Exception::getType() const noexcept
{
    return "Woon2 Exception";
}

int Woon2Exception::
getLineNum() const noexcept
{
    return lineNum_;
}

const std::string&Woon2Exception::getFileStr() const noexcept
{
    return fileStr_;
}

std::string Woon2Exception::getMetaStr() const noexcept
{
    auto oss = std::ostringstream();
    
    oss << "[File] " << fileStr_ << '\n'
        << "[Line] " << lineNum_ << '\n';
    return oss.str();
}