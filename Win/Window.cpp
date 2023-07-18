#include "Window.hpp"

#include <sstream>

namespace Win32
{

WindowException::WindowException(int lineNum, const char* fileStr,
    HRESULT hr) noexcept
    : Woon2Exception(lineNum, fileStr), hr_(hr)
{}

const std::string WindowException::translateErrorCode(
    HRESULT hr ) noexcept
{
    char* pMsgBuf = nullptr;

    auto msgLen = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, hr, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
        reinterpret_cast<LPSTR>( &pMsgBuf ), 0, nullptr
    );

    if (!msgLen) {
        return "unidentified error code";
    }

    auto errorStr = std::string(pMsgBuf, msgLen);
    LocalFree(pMsgBuf);

    return errorStr;
}

const char* WindowException::what() const noexcept  // overriden
{
    std::ostringstream oss{};

    oss << getType() << '\n';

    oss << "[Error Code] " << errorCode() << '\n'
        << "[Description] " << errorStr() << '\n'
        << this->getMetaStr() << '\n';

    whatBuffer_ = oss.str();
    return whatBuffer_.c_str();
}

const char* WindowException::getType() const noexcept // overriden
{
    return "Window Exception";
}

HRESULT WindowException::errorCode() const noexcept
{
    return hr_;
}

const std::string WindowException::errorStr() const noexcept
{
    return translateErrorCode( errorCode() );
}

} // namespace Win32