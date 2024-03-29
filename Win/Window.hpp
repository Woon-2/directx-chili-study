#ifndef __Window
#define __Window

#include <Windows.h>

#include <string>
#include <string_view>
#include <memory>
#include <concepts>
#include <list>
#include <optional>

#include "AdditionalConcepts.hpp"
#include "Woon2Exception.hpp"

#define WND_EXCEPT(hr) Win32::WindowException(__LINE__, __FILE__, (hr))
#define WND_THROW_FAILED(hrcall)    \
    if ( HRESULT hr = (hrcall); hr < 0 ) \
        throw WND_EXCEPT(hr)
#define WND_LAST_EXCEPT() WND_EXCEPT( GetLastError() )

namespace Win32
{

class WindowException : public Woon2Exception
{
public:
    WindowException( int lineNum, const char* fileStr,
        HRESULT hr ) noexcept;

    const char* what() const noexcept override;
    const char* type() const noexcept override;

    HRESULT errorCode() const noexcept;
    const std::string errorStr() const noexcept;

    static const std::string
        translateErrorCode( HRESULT hr ) noexcept;

private:
    HRESULT hr_;
};

template <class T>
concept Win32Char = contains<T, CHAR, WCHAR>;

struct WndFrame
{
    int x;
    int y;
    int width;
    int height;

    operator RECT() const noexcept {
        return RECT{x, y, x + width, y + height};
    }
};

struct Message
{
    UINT type;
    WPARAM wParam;
    LPARAM lParam;
};

template <class Traits, class ... Args>
concept canRegist = requires (Args&& ... args) {
    Traits::regist( std::forward<Args>(args)... );
};

template <class Traits, class ... Args>
concept canCreate = requires (Args&& ... args) {
    { Traits::create( std::forward<Args>(args)... ) } -> std::same_as<HWND>;
};

template <class Traits, class ... Args>
concept canShow = requires (Args&& ... args) {
    Traits::show( std::forward<Args>(args)... );
};

template <class Traits, class ... Args>
concept canDestroy = requires (Args&& ... args) {
    Traits::destroy( std::forward<Args>(args)... );
};

template <class Traits, class ... Args>
concept canUnregist = requires (Args&& ... args) {
    Traits::unregist( std::forward<Args>(args)... );
};

class IMsgHandler {
public:
    virtual ~IMsgHandler() {}
    virtual std::optional<LRESULT> operator()(const Message& msg) = 0;
};

template <class Wnd>
class MsgHandler : public IMsgHandler
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

    virtual std::optional<LRESULT> operator()(const Message& msg) = 0;

protected:
    const Wnd& window() const noexcept { return window_; }
    Wnd& window() noexcept { return window_; }

private:
    Wnd& window_;
};

template <class Wnd>
class BasicMsgHandler : public MsgHandler<Wnd>
{
public:
    using MsgHandler<Wnd>::window;
    using MyChar = typename Wnd::MyChar;

    BasicMsgHandler(Wnd& wnd)
        : MsgHandler<Wnd>(wnd)
    {}

    std::optional<LRESULT> operator()(const Message& msg) override;
};

template <class Traits>
class Window
{
public:
    using MyType = Window<Traits>;
    using MyTraits = Traits;
    using MyChar = Traits::MyChar;
    using MyString = std::basic_string<MyChar>;
    using MyStringView = std::basic_string_view<MyChar>;

    friend MyTraits;

    Window() requires false;
    ~Window() requires canDestroy<Traits, HWND>
    { 
        close();
    }

    template <class ... Args>
    requires canRegist<Traits, HINSTANCE>
        && canCreate<Traits, HINSTANCE, Window<Traits>*, Args...>
    Window(Args&& ... args);
    Window(const Window&) requires false;
    Window(Window&&) requires false;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&) = delete;

    template <class ... Args>
    requires canRegist<Traits, HINSTANCE>
        && canCreate<Traits, HINSTANCE, Window<Traits>*, Args...>
    void open(Args&& ... args);

    void close() requires canDestroy<Traits, HWND> {
        if ( nativeHandle() ) {
            Traits::destroy( nativeHandle() );
            hWnd_ = nullptr;
        }
    }

    static void setHInst(HINSTANCE hInstance) noexcept
    { hInst = hInstance; }
    static HINSTANCE getHInst() noexcept { return hInst; }
    void msgLoop();
    std::optional<int> processMessages();

    HWND nativeHandle() const noexcept { return hWnd_; }
    auto& msgHandlers() noexcept { return msgHandlers_; }
    const auto& msgHandlers() const noexcept { return msgHandlers_; }
    MyStringView title() const noexcept { return title_; }
    void setTitle(const MyString& windowTitle)
    {
        title_ = windowTitle;
        setNativeTitle( title().data() );
    }
    void setTitle(MyString&& windowTitle)
    {
        title_ = std::move(windowTitle);
        setNativeTitle( title().data() );
    }
    void show(int nCmdShow) requires canShow<Traits, HWND, int>
    {
        Traits::show( nativeHandle(), nCmdShow );
    }
    
private:
    static LRESULT CALLBACK wndProcSetupHandler(HWND hWnd, UINT type,
        WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK wndProcCallHandler(HWND hWnd, UINT type,
        WPARAM wParam, LPARAM lParam);

    void setNativeTitle(const MyChar* title)
    {
        bool bFine = false;

        if constexpr ( std::is_same_v<MyChar, CHAR> ) {
            bFine = SetWindowTextA( nativeHandle(), title );
        }
        else /* WCHAR */ {
            bFine = SetWindowTextW( nativeHandle(), title );
        }

        if (!bFine) [[unlikely]] {
            throw WND_LAST_EXCEPT();
        }
    }

    static bool bRegist;
    static HINSTANCE hInst;

    MyString title_;
    WndFrame frame_;
    std::list< std::unique_ptr<IMsgHandler> > msgHandlers_;
    HWND hWnd_;
};

template <Win32Char CharT>
struct BasicWindowTraits
{
public:
    using MyWindow = Window< BasicWindowTraits >;
    using MyChar = CharT;
    using MyString = std::basic_string<MyChar>;
    using MyStringView = std::basic_string_view<MyChar>;

    static constexpr const MyStringView clsName() noexcept
    {
        if constexpr ( std::is_same_v<MyChar, CHAR> ) {
            return "WT";
        }
        else /* WCHAR */ {
            return L"WT";
        }
    }
    static constexpr const MyStringView defWndName() noexcept
    {
        if constexpr ( std::is_same_v<MyChar, CHAR> ) {
            return "Window";
        }
        else /* WCHAR */ {
            return L"Window";
        }
    }
    static constexpr const WndFrame defWndFrame() noexcept
    {
        return WndFrame{ .x=200, .y=200, .width=800, .height=600 };
    }

    static void regist(HINSTANCE hInst);
    static void unregist(HINSTANCE hInst);
    static HWND create(HINSTANCE hInst, MyWindow* pWnd);
    static HWND create(HINSTANCE hInst, MyWindow* pWnd, MyStringView wndName);
    static HWND create(HINSTANCE hInst, MyWindow* pWnd,
        const WndFrame& wndFrame);
    static HWND create(HINSTANCE hInst, MyWindow* pWnd, MyStringView wndName,
        const WndFrame& wndFrame);
    static void destroy(HWND hWnd)
    {
        auto bFine = DestroyWindow(hWnd);

        if (!bFine) {
            throw WND_LAST_EXCEPT();
        }
    }
    static void show(HWND hWnd, int nCmdShow) { ShowWindow(hWnd, nCmdShow); }
};

template <class Traits>
bool Window<Traits>::bRegist = false;

template <class Traits>
HINSTANCE Window<Traits>::hInst = nullptr;

}   // namespace Win32

#include "Window.inl"

#endif  // __Window