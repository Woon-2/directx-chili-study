#ifndef __Window
#define __Window

#include "WindowBase.hpp"

#include <string>
#include <memory>

#include "Keyboard.hpp"

class Window : public WindowBase<Window, wchar_t>
{
private:
    class WindowClass;

public:
    friend class WindowBase;

    Window(const RECT& rect, const wchar_t* name);
    Window(int left, int top, int width, int height, const wchar_t* name);
    Window(int width, int height, const wchar_t* name);
    Window(const RECT& rect, const std::wstring& name);
    Window(int left, int top, int width, int height, const std::wstring& name);
    Window(int width, int height, const std::wstring& name);

    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    HWND get() const noexcept;
    void setTitle(const std::wstring& title);
    void setTitle(std::wstring&& title) noexcept;
    const std::wstring& getTitle() const noexcept;

    Keyboard kbd;

private:
    static std::unique_ptr<WindowClass> pWindowClass;

    static void initWindowClass();
    LRESULT handleMsg( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

    RECT region_;
    HWND hWnd_;
    std::wstring title_;
};

class Window::WindowClass
{
public:
    WindowClass(const wchar_t* name);
    WindowClass(const std::wstring& name);
    ~WindowClass();
    WindowClass(const WindowClass&) = delete;
    WindowClass& operator=(const WindowClass&) = delete;

    HINSTANCE getInst() const;
    const std::wstring& getName() const;

private:
    std::wstring name_;
    HINSTANCE hInst_;

};

#endif // __Window