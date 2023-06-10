#include "WindowBase.hpp"

WindowException::WindowException( int lineNum, const char* fileStr,
    HRESULT hr ) noexcept
    : Woon2Exception(lineNum, fileStr), hr_(hr)
{}

std::string WindowException::translateErrorCode(HRESULT hr) noexcept
{
    char* pMsgBuf = nullptr;

    auto msgLen = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, hr, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
        reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
    );

    if (!msgLen) {
        return "Unidentified error code";
    }

    auto errorStr = std::string(pMsgBuf, msgLen);
    LocalFree(pMsgBuf);

    return errorStr;
}

const char* WindowException::what() const noexcept
{
    auto oss = std::ostringstream();
    oss << getType() << '\n';

    oss << "[Error Code] " << getErrorCode() << '\n'
        << "[Description] " << getErrorStr() << '\n'
        << this->getMetaStr() << '\n';

    whatBuffer_ = oss.str();
    return whatBuffer_.c_str();
}

const char* WindowException::getType() const noexcept
{
    return "Window Exception";
}

HRESULT WindowException::getErrorCode() const noexcept
{
    return hr_;
}

std::string WindowException::getErrorStr() const noexcept
{
    return translateErrorCode( getErrorCode() );
}