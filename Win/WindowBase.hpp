#ifndef __WindowBase
#define __WindowBase

#include <Windows.h>
#include <string>

#include "Woon2Exception.hpp"


#define WND_EXCEPT(hr) WindowException(__LINE__, __FILE__, hr)

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

class WindowException : public Woon2Exception
{
public:
    WindowException( int lineNum, const char* fileStr, HRESULT hr ) noexcept;

    static std::string translateErrorCode( HRESULT hr ) noexcept;

    const char* what() const noexcept override;
    virtual const char* getType() const noexcept override;
    HRESULT getErrorCode() const noexcept;
    std::string getErrorStr() const noexcept;

private:
    HRESULT hr_;

};

#include "WindowBase.inl"

#endif  // __WindowBase