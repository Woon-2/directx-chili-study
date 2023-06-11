#ifndef __WindowBase
#define __WindowBase

#include <Windows.h>
#include <string>

#include "Woon2Exception.hpp"


#define WND_EXCEPT(hr) WindowException(__LINE__, __FILE__, hr)
#define WND_LAST_EXCEPT() WindowException( __LINE__, __FILE__, GetLastError() )

template <class Concrete, class CharT = wchar_t,
    class Traits = std::char_traits<CharT>,
    class Allocator = std::allocator<CharT> >
class WindowBase
{
public:
    using String = std::basic_string<CharT, Traits, Allocator>;

public:
    WindowBase() = default;
    ~WindowBase() = default;
    WindowBase(const WindowBase&) = delete;
    WindowBase& operator=(const WindowBase&) = delete;

    static void injectWndProc(WNDCLASSEXW& wc) noexcept;
    static void injectWndProc(WNDCLASSEXA& wc) noexcept;

private:
    static LRESULT CALLBACK handleMsgSetup( HWND hWnd, UINT msg,
        WPARAM wParam, LPARAM lParam );
    static LRESULT CALLBACK handleMsgThunk( HWND hWnd, UINT msg,
        WPARAM wParam, LPARAM lParam );

    LRESULT handleMsgForward( HWND hWnd, UINT msg,
        WPARAM wParam, LPARAM lParam );
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