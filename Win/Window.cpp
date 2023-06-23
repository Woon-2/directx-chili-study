#include "Window.hpp"

#include <sstream>

#include "WindowsMessageMap.hpp"
#include <Quote.hpp>

std::unique_ptr< Window::WindowClass > Window::pWindowClass = nullptr;

Window::WindowClass::WindowClass(const wchar_t* name)
    : WindowClass( std::wstring(name) )
{}

Window::WindowClass::WindowClass(const std::wstring& name)
    : hInst_( GetModuleHandleW(nullptr) ), name_(name)
{
    if (!hInst_) {
        // on GetModuleHandleW failed
        throw WND_LAST_EXCEPT();
    }

    WNDCLASSEXW wc;

    // fill wc
    wc.cbSize = sizeof(wc);
    wc.cbWndExtra = 0;
    wc.cbClsExtra = 0;
    wc.hbrBackground = nullptr;
    wc.hCursor = nullptr;
    wc.hIcon = nullptr;
    wc.hIcon = static_cast<HICON>(LoadImageW(
        nullptr,
        QUOTEW(RESOURCE_PATH/icon.ico),
        IMAGE_ICON,
        0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE
    ));
    if (!wc.hIcon) {
        // on icon setting failed
        throw WND_LAST_EXCEPT();
    }
    wc.hIconSm = nullptr;
    wc.hInstance = hInst_;
    wc.lpszClassName = name_.c_str();
    wc.style = CS_OWNDC;
    wc.lpszMenuName = nullptr;

    // set WndProc in base class
    injectWndProc(wc);

    // register wc
    if (!RegisterClassExW(&wc)) {
        // on RegisterClassEx failed
        throw WND_LAST_EXCEPT();
    }
}

Window::WindowClass::~WindowClass()
{
    UnregisterClassW(name_.c_str(), hInst_);
}

HINSTANCE Window::WindowClass::getInst() const
{
    return hInst_;
}

const std::wstring& Window::WindowClass::getName() const
{
    return name_;
}

void Window::initWindowClass()
{
    pWindowClass.reset( new WindowClass(L"Simple Window Class") );
}

Window::Window(const RECT& rect, const std::wstring& title)
    : region_(rect), hWnd_(nullptr), title_(title), kbd()
{
    if ( !pWindowClass ) {
        // lazy initialization of window class
        initWindowClass();
    }
    /*
     Client area's size is slightly smaller than whole window area,
     which is caused by the window outline and title bar.
     So adjust the client area's size up to the intended size.
    */
    if ( !AdjustWindowRect( &region_, WS_CAPTION | WS_MINIMIZEBOX
        | WS_SYSMENU, false ) ) {
        // on AdjustWindowRect failed
        throw WND_LAST_EXCEPT();
    }

    hWnd_ = CreateWindowExW(
        0,
        pWindowClass->getName().c_str(),
        title_.c_str(),
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        region_.left,
        region_.top,
        region_.right - region_.left,
        region_.bottom - region_.top,
        nullptr,
        nullptr,
        pWindowClass->getInst(),
        this
    );

    if (hWnd_ == nullptr) {
        // on window creation failed
        throw WND_LAST_EXCEPT();
    }

    ShowWindow(hWnd_, SW_SHOWDEFAULT);
}

Window::Window(int left, int top, int width, int height, const std::wstring& title)
    : Window( RECT{left, top, left + width, top + height}, title )
{}

Window::Window(int width, int height, const std::wstring& title)
    : Window( 0, 0, width, height, title )
{}

Window::Window(const RECT& rect, const wchar_t* title)
    : Window( rect, std::wstring(title) )
{

}

Window::Window(int left, int top, int width, int height, const wchar_t* title)
    : Window( RECT{left, top, left + width, top + height}, title)
{}

Window::Window(int width, int height, const wchar_t* title)
    : Window( 0, 0, width, height, title )
{}

Window::~Window()
{
    DestroyWindow(hWnd_);
}

HWND Window::get() const noexcept
{
    return hWnd_;
}

void Window::setTitle(const std::wstring& title)
{
    title_ = title;
    SetWindowTextW( hWnd_, title_.c_str() );
}

void Window::setTitle(std::wstring&& title) noexcept
{
    title_ = std::move(title);
    SetWindowTextW( hWnd_, title_.c_str() );
}

const std::wstring& Window::getTitle() const noexcept
{
    return title_;
}

LRESULT Window::handleMsg( HWND hWnd, UINT msg, WPARAM wParam,
    LPARAM lParam )
{
    static WindowsMessageMap wmm;

    OutputDebugStringW( wmm(msg, lParam, wParam).c_str() );

    try {

        switch (msg) {
        case WM_CLOSE:
            PostQuitMessage(69);
            break;

        case WM_KILLFOCUS:
            kbd.disableListening();
            break;

        case WM_SETFOCUS:
            kbd.enableListening();
            break;

        case WM_KEYDOWN: case WM_SYSKEYDOWN:
            kbd.getEventQueue().onKeyPressed( static_cast<
                Keyboard::KeyType>(wParam) );
            break;

        case WM_KEYUP: case WM_SYSKEYUP:
            kbd.getEventQueue().onKeyReleased( static_cast<
                Keyboard::KeyType>(wParam) );
            break;

        case WM_CHAR:
            kbd.getEventQueue().onChar( static_cast<
                Keyboard::KeyType>(wParam) );
            break;

        case WM_LBUTTONDOWN: {
            auto posClicked = MAKEPOINTS(lParam);
            auto oss = std::wostringstream();
            oss << L"Click On (" << posClicked.x << L", "
                << posClicked.y << L')';

            setTitle( oss.str() );

            throw WND_EXCEPT(STG_S_BLOCK);

            break;
        }
        }

    }   // try block
    catch (const Woon2Exception& e) {
        MessageBoxA(nullptr, e.what(), "Woon2 Exception",
            MB_OK | MB_ICONEXCLAMATION);
    }
    catch (const std::exception& e) {
        MessageBoxA(nullptr, e.what(), "Standard Exception",
            MB_OK | MB_ICONEXCLAMATION);
    }
    catch (...) {
        MessageBoxA(nullptr, "no details available",
            "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}