#ifndef __Windowxx
#define __Windowxx

#include <Windows.h>

#include <string>
#include <string_view>
#include <memory>

#include <AdditionalConcepts.hpp>

#include "Woon2Exception.hpp"

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

struct Message
{
    UINT type;
    WPARAM wParam;
    LPARAM lParam;
};

template <class Wnd>
class MsgHandler
{
public:
    using MyWindow = Wnd;

    MsgHandler(Wnd& wnd) noexcept
        : window_(wnd)
    {}
    MsgHandler(const MsgHandler&) = delete;
    MsgHandler(MsgHandler&&) = delete;
    MsgHandler& operator=(const MsgHandler&) = delete;
    MsgHandler& operator=(MsgHandler&&) = delete;
    virtual ~MsgHandler() {}

    virtual LRESULT operator()(const Message& msg) = 0;

protected:
    const Wnd& window() const noexcept { return window_; }
    Wnd& window() noexcept { return window_; }

private:
    Wnd& window_;
};


template <class Traits>
class Window
{
public:
    friend Traits;

    using MyType = Window<Traits>;
    using MyHandler = MsgHandler< Window<Traits> >;
    using MyTraits = Traits;
    using MyChar = Traits::MyChar;

    Window();
    ~Window() { DestroyWindow(hWnd_); }
    template <class ... Args>
    Window(Args&& ... args);
    Window(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&) = delete;

    static void setHInst(HINSTANCE hInstance) noexcept { hInst = hInstance; }
    static HINSTANCE getHInst() noexcept { return hInst; }
    static void msgLoop();

    HWND nativeHandle() noexcept { return hWnd_; }
    template <class MH>
    void setMsgHandler() { pHandleMsg_.reset( new MH(*this) ); }

private:
    static LRESULT CALLBACK wndProcSetupHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK wndProcCallHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    static bool bRegist;
    static HINSTANCE hInst;

    std::unique_ptr<MyHandler> pHandleMsg_;
    HWND hWnd_;
};

template <class Wnd>
class BasicMsgHandler : public MsgHandler<Wnd>
{
public:
    using MsgHandler<Wnd>::window;

    BasicMsgHandler(Wnd& wnd)
        : MsgHandler<Wnd>(wnd)
    {}

    LRESULT operator()(const Message& msg) override;
};

template <Win32Char CharT>
struct BasicWindowTraits
{
    using MyWindow = Window< BasicWindowTraits >;
    using MyChar = CharT;

    static constexpr const std::basic_string_view<CharT> clsName() noexcept;
    static constexpr const std::basic_string_view<CharT> defWndName() noexcept;
    static constexpr const WndFrame defWndFrame() noexcept;
    static void regist(HINSTANCE hInst);
    static void unregist(HINSTANCE hInst);
    static HWND create(HINSTANCE hInst, MyWindow* pWnd);
    static HWND create(HINSTANCE hInst, MyWindow* pWnd, std::basic_string_view<CharT> wndName);
    static HWND create(HINSTANCE hInst, MyWindow* pWnd, const WndFrame& wndFrame);
    static HWND create(HINSTANCE hInst, MyWindow* pWnd, std::basic_string_view<CharT> wndName,
        const WndFrame& wndFrame);
    static void show(HWND hWnd) { ShowWindow(hWnd, SW_SHOWDEFAULT); }
};

template <Win32Char CharT>
struct MainWindowTraits
{
    using MyWindow = Window< MainWindowTraits >;

    static constexpr const std::basic_string_view<CharT> clsName() noexcept;
    static void regist(HINSTANCE hInst);
    static void unregist(HINSTANCE hInst);
    static HWND create(HINSTANCE hInst, MyWindow* pWnd,
        std::basic_string_view<CharT> wndName, const WndFrame& wndFrame);
    static void show(HWND hWnd) { ShowWindow(hWnd, SW_SHOWDEFAULT); }
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