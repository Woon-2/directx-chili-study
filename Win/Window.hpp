#ifndef __Window
#define __Window

#define UNICODE
#define _UNICODE

#include <Windows.h>
#include <tchar.h>
#include <string>
#include <sstream>

#include "WindowsMessageMap.hpp"

#define String std::wstring
#define CharT wchar_t

class Window
{
private:
    class WindowClass;

public:
    Window(int left, int top, int width, int height, const CharT* name);
    Window(int width, int height, const CharT* name);
    Window(const RECT& rect, const CharT* name);
    Window(int left, int top, int width, int height, const String& name);
    Window(int width, int height, const String& name);
    Window(const RECT& rect, const String& name);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    HWND get() const;

private:
    static WindowClass wc;

    static LRESULT CALLBACK HandleMsgSetup( HWND hWnd, UINT msg,
        WPARAM wParam, LPARAM lParam );
    static LRESULT CALLBACK HandleMsgThunk( HWND hWnd, UINT msg,
        WPARAM wParam, LPARAM lParam );

    LRESULT HandleMsg( HWND hWnd, UINT msg, WPARAM wParam,
        LPARAM lParam );

private:
    HWND hWnd_;
    int left_;
    int top_;
    int width_;
    int height_;    
};

class Window::WindowClass
{
public:
    WindowClass(const CharT* name) noexcept;
    WindowClass(const String& name) noexcept;
    ~WindowClass();
    WindowClass(const WindowClass&) = delete;
    WindowClass& operator=(const WindowClass&) = delete;

    HINSTANCE getInst() const noexcept;
    const String& getName() const noexcept;

private:
    void registerWC();

    String name_;
    HINSTANCE hInst_;
};

Window::WindowClass::WindowClass(const CharT* name) noexcept
    : hInst_( GetModuleHandle(nullptr) ), name_( name )
{
    registerWC();
}

Window::WindowClass::WindowClass(const String& name) noexcept
{
    registerWC();
}

Window::WindowClass::~WindowClass()
{
    UnregisterClass(name_.c_str(), hInst_);
}

HINSTANCE Window::WindowClass::getInst() const noexcept
{
    return hInst_;
}

const String& Window::WindowClass::getName() const noexcept
{
    return name_;
}

void Window::WindowClass::registerWC()
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


Window::WindowClass Window::wc( TEXT("DefWindowClass") );



Window::Window(int left, int top, int width, int height, const CharT* name)
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

Window::Window(int width, int height, const CharT* name)
    : Window( 0, 0, width, height, name )
{}

Window::Window(const RECT& rect, const CharT* name)
    : Window( rect.left, rect.top, rect.right - rect.left,
        rect.bottom - rect.top, name )
{}

Window::Window(int left, int top, int width, int height, const String& name)
    : Window( left, top, width, height, name.c_str() )
{}

Window::Window(int width, int height, const String& name)
    : Window( width, height, name.c_str() )
{}

Window::Window(const RECT& rect, const String& name)
    : Window( rect, name.c_str() )
{}

Window::~Window()
{
    DestroyWindow(hWnd_);
}

HWND Window::get() const
{
    return hWnd_;
}

LRESULT CALLBACK Window::HandleMsgSetup( HWND hWnd, UINT msg,
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

    return pWnd->HandleMsg( hWnd, msg, wParam, lParam );
}

LRESULT CALLBACK Window::HandleMsgThunk( HWND hWnd, UINT msg,
    WPARAM wParam, LPARAM lParam )
{
    auto pWnd = reinterpret_cast< Window* >(
        GetWindowLongPtr( hWnd, GWLP_USERDATA )
    );

    return pWnd->HandleMsg( hWnd, msg, wParam, lParam );
}

LRESULT Window::HandleMsg( HWND hWnd, UINT msg, WPARAM wParam,
    LPARAM lParam )
{
    static WindowsMessageMap wmm;

    OutputDebugString( wmm(msg, lParam, wParam).c_str() );

    switch( msg ) {
    case WM_CLOSE:
        PostQuitMessage(69);
        break;

    case WM_KEYUP:
        if (wParam == 'F') {
            SetWindowText(hWnd, TEXT("Danger field"));
        }
        break;

    case WM_CHAR: {
        static auto title = std::wstring();
        title.push_back( static_cast<wchar_t>(wParam) );
        SetWindowText(hWnd, title.c_str());
        break;
    }

    case WM_LBUTTONDOWN: {
        auto posClicked = MAKEPOINTS(lParam);
        auto oss = std::wostringstream();
        oss << TEXT("Click On (") << posClicked.x << TEXT(", ")
            << posClicked.y << TEXT(")");
        
        SetWindowText(hWnd, oss.str().c_str());
        break;
    }
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

#endif