#include "Window.hpp"

Window::Window(int left, int top, int width, int height, wchar_t* name)
    : WindowBase(left, top, width, height, name)
{}

Window::Window(int width, int height, wchar_t* name)
    : WindowBase(width, height, name)
{}

Window::Window(const RECT& rect, wchar_t* name)
    : WindowBase(rect, name)
{}

Window::Window(int left, int top, int width, int height, const std::wstring& name)
    : WindowBase(left, top, width, height, name)
{}

Window::Window(int width, int height, const std::wstring& name)
    : WindowBase(width, height, name)
{}

Window::Window(const RECT& rect, const std::wstring& name)
    : WindowBase(rect, name)
{}


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