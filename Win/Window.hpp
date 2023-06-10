#ifndef __Window
#define __Window

#include "WindowBase.hpp"

class Window : public WindowBase<Window, wchar_t>
{
public:
    friend class WindowBase;

    Window(int left, int top, int width, int height, wchar_t* name);
    Window(int width, int height, wchar_t* name);
    Window(const RECT& rect, wchar_t* name);
    Window(int left, int top, int width, int height, const std::wstring& name);
    Window(int width, int height, const std::wstring& name);
    Window(const RECT& rect, const std::wstring& name);

    ~Window() = default;
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

private:
    LRESULT handleMsg( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
};

#endif // __Window