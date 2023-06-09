#ifndef __Window
#define __Window

#include "WindowBase.hpp"

class WindowImpl : public Window<WindowImpl, wchar_t>
{
public:
    friend class Window;

    WindowImpl(int left, int top, int width, int height, wchar_t* name);
    WindowImpl(int width, int height, wchar_t* name);
    WindowImpl(const RECT& rect, wchar_t* name);
    WindowImpl(int left, int top, int width, int height, const std::wstring& name);
    WindowImpl(int width, int height, const std::wstring& name);
    WindowImpl(const RECT& rect, const std::wstring& name);

    ~WindowImpl() = default;
    WindowImpl(const WindowImpl&) = delete;
    WindowImpl& operator=(const WindowImpl&) = delete;

private:
    LRESULT HandleMsg( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
};

#endif // __Window