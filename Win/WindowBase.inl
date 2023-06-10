#include "WindowBase.hpp"

#include <sstream>
#include <type_traits>

template <class Concrete, class CharT, class Traits, class Allocator>
WindowBase<Concrete, CharT, Traits, Allocator>::WindowClass::
WindowClass(const CharT* name) noexcept
    : hInst_( GetModuleHandle(nullptr) ), name_( name )
{
    registerWC();
}

template <class Concrete, class CharT, class Traits, class Allocator>
WindowBase<Concrete, CharT, Traits, Allocator>::WindowClass::
WindowClass(const String& name) noexcept
{
    registerWC();
}

template <class Concrete, class CharT, class Traits, class Allocator>
WindowBase<Concrete, CharT, Traits, Allocator>::WindowClass::
~WindowClass()
{
    UnregisterClass(name_.c_str(), hInst_);
}

template <class Concrete, class CharT, class Traits, class Allocator>
HINSTANCE WindowBase<Concrete, CharT, Traits, Allocator>::WindowClass::
getInst() const noexcept
{
    return hInst_;
}

template <class Concrete, class CharT, class Traits, class Allocator>
const typename WindowBase<Concrete, CharT, Traits, Allocator>::String&
WindowBase<Concrete, CharT, Traits, Allocator>::WindowClass::
getName() const noexcept
{
    return name_;
}

template <class Concrete, class CharT, class Traits, class Allocator>
void WindowBase<Concrete, CharT, Traits, Allocator>::
WindowClass::registerWC()
{
    auto wc = std::conditional_t<
        std::is_same_v<CharT, char>, WNDCLASSEXA,
        std::conditional_t<
            std::is_same_v<CharT, wchar_t>, WNDCLASSEXW,
            std::false_type
        >
    >();

    if constexpr( std::is_same_v<decltype(wc), std::false_type> ) {
        static_assert("There's no matching character type on Window to CharT.");
    }

    wc.cbSize = sizeof(wc);
    wc.cbWndExtra = 0;
    wc.cbClsExtra = 0;
    wc.hbrBackground = nullptr;
    wc.hCursor = nullptr;
    wc.hIcon = nullptr;
    wc.hIconSm = nullptr;
    wc.hInstance = hInst_;
    wc.lpfnWndProc = handleMsgSetup;
    wc.lpszClassName = name_.c_str();

    RegisterClassEx(&wc);
}

template <class Concrete, class CharT, class Traits, class Allocator>
WindowBase<Concrete, CharT, Traits, Allocator>::
WindowBase(const RECT& rect, const CharT* name)
    : region_(rect)
{
    /*
     Client area's size is slightly smaller than whole window area,
     which is caused by the window outline and title bar.
     So adjust the client area's size up to the intended size.
    */
    AdjustWindowRect( &region_, WS_CAPTION | WS_MINIMIZEBOX
        | WS_SYSMENU, false );

#define __CWArgList \
    0,  \
    wc.getName().c_str(),   \
    name,   \
    WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,   \
    region_.left,   \
    region_.top,    \
    region_.right - region_.left,   \
    region_.bottom - region_.top,   \
    nullptr,    \
    nullptr,    \
    wc.getInst(),   \
    this

    if constexpr( std::is_same_v<CharT, char> ) {
        hWnd_ = CreateWindowExA(__CWArgList);
    }
    else if constexpr( std::is_same_v<CharT, wchar_t> ) {
        hWnd_ = CreateWindowExW(__CWArgList);
    }
    else {
        static_assert("There's no matching character type on Window to CharT.");
    }

#undef __CWArgList

    ShowWindow(hWnd_, SW_SHOWDEFAULT);
}

template <class Concrete, class CharT, class Traits, class Allocator>
WindowBase<Concrete, CharT, Traits, Allocator>::
WindowBase(int left, int top, int width, int height, const CharT* name)
    : WindowBase( RECT{left, top, left + width, top + height}, name )
{}

template <class Concrete, class CharT, class Traits, class Allocator>
WindowBase<Concrete, CharT, Traits, Allocator>::
WindowBase(int width, int height, const CharT* name)
    : WindowBase( 0, 0, width, height, name )
{}

template <class Concrete, class CharT, class Traits, class Allocator>
WindowBase<Concrete, CharT, Traits, Allocator>::
WindowBase(const RECT& rect, const String& name)
    : WindowBase( rect, name.c_str() )
{}

template <class Concrete, class CharT, class Traits, class Allocator>
WindowBase<Concrete, CharT, Traits, Allocator>::
WindowBase(int left, int top, int width, int height, const String& name)
    : WindowBase( left, top, width, height, name.c_str() )
{}

template <class Concrete, class CharT, class Traits, class Allocator>
WindowBase<Concrete, CharT, Traits, Allocator>::
WindowBase(int width, int height, const String& name)
    : WindowBase( width, height, name.c_str() )
{}

template <class Concrete, class CharT, class Traits, class Allocator>
WindowBase<Concrete, CharT, Traits, Allocator>::~WindowBase()
{
    DestroyWindow(hWnd_);
}

template <class Concrete, class CharT, class Traits, class Allocator>
HWND WindowBase<Concrete, CharT, Traits, Allocator>::get() const
{
    return hWnd_;
}

template <class Concrete, class CharT, class Traits, class Allocator>
LRESULT CALLBACK WindowBase<Concrete, CharT, Traits, Allocator>::
handleMsgSetup( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    using MyCreateStruct = std::conditional_t<
        std::is_same_v<CharT, char>, CREATESTRUCTA,
        std::conditional_t<
            std::is_same_v<CharT, wchar_t>, CREATESTRUCTW,
            std::false_type
        >
    >;

    if constexpr( std::is_same_v<MyCreateStruct, std::false_type> ) {
        static_assert("There's no matching character type on Window to CharT.");
    }

    if (msg != WM_NCCREATE) {
        return DefWindowProc( hWnd, msg, wParam, lParam );
    }

    // extract ptr to window from creation data
    const auto pCreate = reinterpret_cast<
        const MyCreateStruct*>( lParam );
    auto pWnd = static_cast<WindowBase*>( pCreate->lpCreateParams );
    
    // make WinAPI-managed user data to store the ptr to window
    SetWindowLongPtr( hWnd, GWLP_USERDATA,
        reinterpret_cast<LONG_PTR>( pWnd ) );
    // setup is done,
    // change message handler into regular one.
    SetWindowLongPtr( hWnd, GWLP_WNDPROC,
        reinterpret_cast<LONG_PTR>( &WindowBase::handleMsgThunk ) );

    return pWnd->handleMsgForward( hWnd, msg, wParam, lParam );
}

template <class Concrete, class CharT, class Traits, class Allocator>
LRESULT CALLBACK WindowBase<Concrete, CharT, Traits, Allocator>::
handleMsgThunk( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    auto pWnd = reinterpret_cast< WindowBase* >(
        GetWindowLongPtr( hWnd, GWLP_USERDATA )
    );

    return pWnd->handleMsgForward( hWnd, msg, wParam, lParam );
}

template <class Concrete, class CharT, class Traits, class Allocator>
LRESULT WindowBase<Concrete, CharT, Traits, Allocator>::
handleMsgForward( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    return static_cast<Concrete*>(this)->handleMsg(hWnd, msg, wParam, lParam);
}

template < class CharT, class Traits, class Allocator >
WindowException<CharT, Traits, Allocator>::
WindowException( int lineNum, const CharT* fileStr, HRESULT hr ) noexcept
    : Woon2Exception(lineNum, fileStr), hr_(hr)
{}

template < class CharT, class Traits, class Allocator >
typename WindowException<CharT, Traits, Allocator>::String
WindowException<CharT, Traits, Allocator>::
translateErrorCode(HRESULT hr) noexcept
{
    CharT* pMsgBuf = nullptr;

    auto msgLen = DWORD(0);

    if constexpr( std::is_same_v<CharT, char> ) {
        msgLen = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, hr, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
            reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
        );

        if (!msgLen) {
            return "Unidentified error code";
        }
    }
    else if constexpr( std::is_same_v<CharT, wchar_t> ) {
        msgLen = FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, hr, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
            reinterpret_cast<LPWSTR>(&pMsgBuf), 0, nullptr
        );

        if (!msgLen) {
            return L"Unidentified error code";
        }
    }
    else {
        static_assert("There's no matching character type on WindowException to CharT.");
        return String();
    }

    auto errorStr = String(pMsgBuf, msgLen);
    LocalFree(pMsgBuf);

    return errorStr;
}

template < class CharT, class Traits, class Allocator >
const CharT* WindowException<CharT, Traits, Allocator>::
what() const noexcept
{
    auto oss = std::basic_ostringstream<CharT, Traits, Allocator>;
    oss << getType() << '\n';

    if constexpr( std::is_same_v<CharT, char> ) {
        oss << "[Error Code] " << getErrorCode() << '\n'
            << "[Description] " << getErrorStr() << '\n'
            << this->getMetaStr() << '\n';
    }
    else if constexpr( std::is_same_v<CharT, wchar_t> ) {
        oss << L"[Error Code] " << getErrorCode() << '\n'
            << L"[Description] " << getErrorStr() << '\n'
            << this->getMetaStr() << '\n';
    }
    else {
        static_assert("There's no matching character type on WindowException to CharT.");
        return nullptr;
    }
}

template < class CharT, class Traits, class Allocator >
const CharT* WindowException<CharT, Traits, Allocator>::
getType() const noexcept
{
    if constexpr( std::is_same_v<CharT, char> ) {
        return "Window Exception";
    }
    else if constexpr( std::is_same_v<CharT, wchar_t> ) {
        return L"Window Exception";
    }
    else {
        static_assert("There's no matching character type on WindowException to CharT.");
        return nullptr;
    }
}

template < class CharT, class Traits, class Allocator >
HRESULT WindowException<CharT, Traits, Allocator>::
getErrorCode() const noexcept
{
    return hr_;
}

template < class CharT, class Traits, class Allocator >
typename WindowException<CharT, Traits, Allocator>::String
WindowException<CharT, Traits, Allocator>::
getErrorStr() const noexcept
{
    return translateErrorCode( getErrorCode() );
}