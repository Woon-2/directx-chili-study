#ifndef __Windowxx
#define __Windowxx

#include "Woon2Exception.hpp"
#include <Windows.h>

#include <AdditionalConcepts.hpp>
#include <StringLike.hpp>

#include <string_view>

#define WND_EXCEPT(hr) WindowException(__LINE__, __FILE__, hr)
#define WND_LAST_EXCEPT() WND_EXCEPT( GetLastError() )

namespace Win32
{

template <class T>
concept Win32Char = contains<T, CHAR, WCHAR>;

struct WndFrame
{
    int x;
    int y;
    int width;
    int height;
};

template <Win32Char CharT>
struct BasicWindowTraits
{
    static constexpr const std::basic_string_view<CharT> clsName() noexcept;
    static constexpr const std::basic_string_view<CharT> defWndName() noexcept;
    static constexpr const WndFrame defWndFrame() noexcept;
    static void regist(HINSTANCE hInst);
    static void unregist(HINSTANCE hInst);
    static HWND create(HINSTANCE hInst);
    static HWND create(HINSTANCE hInst, std::basic_string_view<CharT> wndName);
    static HWND create(HINSTANCE hInst, const WndFrame& wndFrame);
    static HWND create(HINSTANCE hInst, std::basic_string_view<CharT> wndName,
        const WndFrame& wndFrame);
    static void show(HWND hWnd);
};

template <Win32Char CharT>
struct MainWindowTraits
{
    static constexpr const std::basic_string_view<CharT> clsName() noexcept;
    static void regist(HINSTANCE hInst, WNDPROC wndProc);
    static void unregist(HINSTANCE hInst);
    static HWND create(HINSTANCE hInst, std::basic_string_view<CharT> wndName,
        const WndFrame& wndFrame, LPVOID lpParam);
    static void show(HWND hWnd); 
};

template <class Traits>
class Window
{
public:
    using traits_type = Traits;

    Window();
    ~Window();
    template <class ... Args>
    Window(Args&& ... args);
    Window(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&) = delete;

    static void setHInst(HINSTANCE hInstance) noexcept;
    static HINSTANCE getHInst() noexcept;

    static void msgLoop();

private:
    static bool bRegist;
    static HINSTANCE hInst;
    HWND hWnd_;
};

class WindowException : public Woon2Exception
{
public:
    WindowException( int lineNum, const char* fileStr, HRESULT hr ) noexcept;

    const char* what() const noexcept override;
    const char* getType() const noexcept override;

    HRESULT errorCode() const noexcept;
    const std::string errorStr() const noexcept;

    static const std::string translateErrorCode( HRESULT hr ) noexcept;

private:
    HRESULT hr_;
};

template <class Traits>
bool Window<Traits>::bRegist = false;

template <class Traits>
HINSTANCE Window<Traits>::hInst = nullptr;

}   // namespace Win32

#include "WindowBasexx.inl"

#endif  // __Windowxx