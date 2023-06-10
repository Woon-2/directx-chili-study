#ifndef __WindowBase
#define __WindowBase

#include <Windows.h>
#include <tchar.h>
#include <string>

#include "WindowsMessageMap.hpp"

template <class Concrete, class CharT = wchar_t,
    class Traits = std::char_traits<CharT>,
    class Allocator = std::allocator<CharT> >
class WindowBase
{
public:
    using String = std::basic_string<CharT, Traits, Allocator>;

private:
    class WindowClass;

public:
    WindowBase(const RECT& rect, const CharT* name);
    WindowBase(int left, int top, int width, int height, const CharT* name);
    WindowBase(int width, int height, const CharT* name);
    WindowBase(const RECT& rect, const String& name);
    WindowBase(int left, int top, int width, int height, const String& name);
    WindowBase(int width, int height, const String& name);
    ~WindowBase();

    WindowBase(const WindowBase&) = delete;
    WindowBase& operator=(const WindowBase&) = delete;

    HWND get() const;

private:
    static WindowClass wc;

    static LRESULT CALLBACK handleMsgSetup( HWND hWnd, UINT msg,
        WPARAM wParam, LPARAM lParam );
    static LRESULT CALLBACK handleMsgThunk( HWND hWnd, UINT msg,
        WPARAM wParam, LPARAM lParam );

    LRESULT handleMsgForward( HWND hWnd, UINT msg,
        WPARAM wParam, LPARAM lParam );

private:
    HWND hWnd_;
    RECT region_; 
};

template <class Concrete, class CharT, class Traits, class Allocator>
class WindowBase<Concrete, CharT, Traits, Allocator>::WindowClass
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