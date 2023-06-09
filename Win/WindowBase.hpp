#ifndef __WindowBase
#define __WindowBase

#include <Windows.h>
#include <tchar.h>
#include <string>
#include <sstream>

#include "WindowsMessageMap.hpp"

template <class Concrete, class CharT = wchar_t,
    class Traits = std::char_traits<CharT>, class Allocator = std::allocator<CharT> >
class Window
{
public:
    using String = std::basic_string<CharT, Traits, Allocator>;

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

    LRESULT ForwardMsgToHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

private:
    HWND hWnd_;
    int left_;
    int top_;
    int width_;
    int height_;    
};

template <class Concrete, class CharT, class Traits, class Allocator>
class Window<Concrete, CharT, Traits, Allocator>::WindowClass
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

#include "WindowBase.inl"

#endif  // __WindowBase