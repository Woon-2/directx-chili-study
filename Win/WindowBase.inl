#include "WindowBase.hpp"

template <class Concrete, class CharT, class Traits, class Allocator>
typename Window<Concrete, CharT, Traits, Allocator>::WindowClass
Window<Concrete, CharT, Traits, Allocator>::wc( TEXT("DefWindowClass") );

template <class Concrete, class CharT, class Traits, class Allocator>
Window<Concrete, CharT, Traits, Allocator>::WindowClass::
WindowClass(const CharT* name) noexcept
    : hInst_( GetModuleHandle(nullptr) ), name_( name )
{
    registerWC();
}

template <class Concrete, class CharT, class Traits, class Allocator>
Window<Concrete, CharT, Traits, Allocator>::WindowClass::
WindowClass(const String& name) noexcept
{
    registerWC();
}

template <class Concrete, class CharT, class Traits, class Allocator>
Window<Concrete, CharT, Traits, Allocator>::WindowClass::
~WindowClass()
{
    UnregisterClass(name_.c_str(), hInst_);
}

template <class Concrete, class CharT, class Traits, class Allocator>
HINSTANCE Window<Concrete, CharT, Traits, Allocator>::WindowClass::
getInst() const noexcept
{
    return hInst_;
}

template <class Concrete, class CharT, class Traits, class Allocator>
const typename Window<Concrete, CharT, Traits, Allocator>::String&
Window<Concrete, CharT, Traits, Allocator>::WindowClass::
getName() const noexcept
{
    return name_;
}

template <class Concrete, class CharT, class Traits, class Allocator>
void Window<Concrete, CharT, Traits, Allocator>::WindowClass::registerWC()
{
    auto wc = WNDCLASSEX();

    wc.cbSize = sizeof(wc);
    wc.cbWndExtra = 0;
    wc.cbClsExtra = 0;
    wc.hbrBackground = nullptr;
    wc.hCursor = nullptr;
    wc.hIcon = nullptr;
    wc.hIconSm = nullptr;
    wc.hInstance = hInst_;
    wc.lpfnWndProc = HandleMsgSetup;
    wc.lpszClassName = name_.c_str();

    RegisterClassEx(&wc);
}

template <class Concrete, class CharT, class Traits, class Allocator>
Window<Concrete, CharT, Traits, Allocator>::Window(int left, int top, int width, int height, const CharT* name)
    : left_(left), top_(top), width_(width), height_(height)
{
    /*
     Client area's size is slightly smaller than whole window area,
     which is caused by the window outline and title bar.
     So adjust the client area's size up to the intended size.
    */

    auto wr = RECT{ left, top, left + width, top + height };
    AdjustWindowRect( &wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, false );

    hWnd_ = CreateWindow(
        wc.getName().c_str(),
        name,
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        wr.left,
        wr.top,
        wr.right - wr.left,
        wr.bottom - wr.top,
        nullptr,
        nullptr,
        wc.getInst(),
        this
    );

    ShowWindow(hWnd_, SW_SHOWDEFAULT);
}

template <class Concrete, class CharT, class Traits, class Allocator>
Window<Concrete, CharT, Traits, Allocator>::Window(int width, int height, const CharT* name)
    : Window( 0, 0, width, height, name )
{}

template <class Concrete, class CharT, class Traits, class Allocator>
Window<Concrete, CharT, Traits, Allocator>::Window(const RECT& rect, const CharT* name)
    : Window( rect.left, rect.top, rect.right - rect.left,
        rect.bottom - rect.top, name )
{}

template <class Concrete, class CharT, class Traits, class Allocator>
Window<Concrete, CharT, Traits, Allocator>::Window(int left, int top, int width, int height, const String& name)
    : Window( left, top, width, height, name.c_str() )
{}

template <class Concrete, class CharT, class Traits, class Allocator>
Window<Concrete, CharT, Traits, Allocator>::Window(int width, int height, const String& name)
    : Window( width, height, name.c_str() )
{}

template <class Concrete, class CharT, class Traits, class Allocator>
Window<Concrete, CharT, Traits, Allocator>::Window(const RECT& rect, const String& name)
    : Window( rect, name.c_str() )
{}

template <class Concrete, class CharT, class Traits, class Allocator>
Window<Concrete, CharT, Traits, Allocator>::~Window()
{
    DestroyWindow(hWnd_);
}

template <class Concrete, class CharT, class Traits, class Allocator>
HWND Window<Concrete, CharT, Traits, Allocator>::get() const
{
    return hWnd_;
}

template <class Concrete, class CharT, class Traits, class Allocator>
LRESULT CALLBACK Window<Concrete, CharT, Traits, Allocator>::HandleMsgSetup( HWND hWnd, UINT msg,
    WPARAM wParam, LPARAM lParam )
{
    if (msg != WM_NCCREATE) {
        return DefWindowProc( hWnd, msg, wParam, lParam );
    }

    // extract ptr to window from creation data
    const auto pCreate = reinterpret_cast<
        const CREATESTRUCT*>( lParam );
    auto pWnd = static_cast<Window*>( pCreate->lpCreateParams );
    
    // make WinAPI-managed user data to store the ptr to window
    SetWindowLongPtr( hWnd, GWLP_USERDATA,
        reinterpret_cast<LONG_PTR>( pWnd ) );
    // setup is done,
    // change message handler into regular one.
    SetWindowLongPtr( hWnd, GWLP_WNDPROC,
        reinterpret_cast<LONG_PTR>( &Window::HandleMsgThunk ) );

    return pWnd->ForwardMsgToHandler( hWnd, msg, wParam, lParam );
}

template <class Concrete, class CharT, class Traits, class Allocator>
LRESULT CALLBACK Window<Concrete, CharT, Traits, Allocator>::HandleMsgThunk( HWND hWnd, UINT msg,
    WPARAM wParam, LPARAM lParam )
{
    auto pWnd = reinterpret_cast< Window* >(
        GetWindowLongPtr( hWnd, GWLP_USERDATA )
    );

    return pWnd->ForwardMsgToHandler( hWnd, msg, wParam, lParam );
}

template <class Concrete, class CharT, class Traits, class Allocator>
LRESULT Window<Concrete, CharT, Traits, Allocator>::ForwardMsgToHandler( HWND hWnd, UINT msg,
    WPARAM wParam, LPARAM lParam )
{
    return static_cast<Concrete*>(this)->HandleMsg(hWnd, msg, wParam, lParam);
}