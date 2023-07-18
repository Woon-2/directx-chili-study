#include "WindowBasexx.hpp"

#include <type_traits>

#include <Invocable.hpp>

namespace Win32
{

template <class Traits>
Window<Traits>::Window()
{
    static_assert(
        is_invocation_valid(
            []() -> decltype( Traits::regist( getHInst() ) ){}
        )
        || is_invocation_valid(
            []() -> decltype( Traits::create( getHInst(), this ) ){}
        ),
        "default constructor call of Window isn't valid. "
        "invocations of Traits::regist( getHInst() ) and "
        "Traits::create( getHInst(), this ) must be valid."
    );

    if (!bRegist) [[unlikely]] {
        Traits::regist( getHInst() );
        bRegist = true;
    }

    setMsgHandler< BasicMsgHandler< MyType > >();

    // enclose this in the Win32 Window,
    // which makes getting Window object from Win32 window handle possible.
    hWnd_ = Traits::create( getHInst(), this );
    Traits::show(hWnd_);
}

template <class Traits>
template <class ... Args>
Window<Traits>::Window(Args&& ... args)
{
    static_assert(
        is_invocation_valid(
            []() -> decltype( Traits::regist( getHInst() ) ){}
        )
        || is_invocation_valid(
            []() -> decltype( Traits::create( getHInst(), this,
                std::forward<Args>(args)... ) ){}
        ),
        "default constructor call of Window isn't valid. "
        "invocations of Traits::regist( getHInst() ) and "
        "Traits::create( getHInst(), this, std::forward<Args>(args)... ) must be valid."
    );

    if (!bRegist) [[unlikely]] {
        Traits::regist( getHInst() );
        bRegist = true;
    }

    setMsgHandler< BasicMsgHandler< MyType > >();

    // enclose this in the Win32 Window,
    // which makes getting Window object from Win32 window handle possible.
    hWnd_ = Traits::create( getHInst(), this, std::forward<Args>(args)... );
    Traits::show(hWnd_);
}

template <class Traits>
void Window<Traits>::msgLoop()
{
    MSG msg;
    BOOL result;

    try {

        while ( ( result = GetMessageW(&msg, nullptr, 0, 0) ) > 0 ) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

    }
    catch (const WindowException& e) {
        MessageBoxA(nullptr, e.what(), "Window Exception",
            MB_OK | MB_ICONEXCLAMATION);
    }
    catch (const std::exception& e) {
        MessageBoxA(nullptr, e.what(), "Standard Exception",
            MB_OK | MB_ICONEXCLAMATION);
    }
    catch(...) {
        MessageBoxA(nullptr, "no details available",
            "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }
}

template <class Traits>
LRESULT Window<Traits>::wndProcSetupHandler(HWND hWnd, UINT msg,
    WPARAM wParam, LPARAM lParam)
{
    constexpr auto& defWindowProc = std::is_same_v<MyChar, CHAR> ?
        DefWindowProcA : DefWindowProcW;

    if (msg != WM_NCCREATE) {
        return defWindowProc(hWnd, msg, wParam, lParam);
    }

    using CreateStruct = std::conditional_t< std::is_same_v<MyChar, CHAR>,
        CREATESTRUCTA, CREATESTRUCTW >;

    // extract ptr to window from creation data
    const auto pCreate = reinterpret_cast< const CreateStruct* >( lParam );
    auto pWnd = static_cast< Window<Traits>* >( pCreate->lpCreateParams );

    constexpr auto& setWindowLongPtr = std::is_same_v<MyChar, CHAR> ?
        SetWindowLongPtrA : SetWindowLongPtrW;

    // make WinAPI-managed user data to store the ptr to window
    setWindowLongPtr( hWnd, GWLP_USERDATA,
        reinterpret_cast<LONG_PTR>( pWnd ) );

    // setup is done,
    // substitute message WndProc with regular one.
    setWindowLongPtr( hWnd, GWLP_WNDPROC,
        reinterpret_cast<LONG_PTR>(wndProcCallHandler) );

    return defWindowProc(hWnd, msg, wParam, lParam); 
}

template <class Traits>
LRESULT Window<Traits>::wndProcCallHandler(HWND hWnd, UINT msg,
    WPARAM wParam, LPARAM lParam)
{
    // fetch ptr to window stored for WinAPI-managed user data
    MyType* pWnd = nullptr;

    constexpr auto& getWindowLongPtr = std::is_same_v<MyChar, CHAR> ?
        GetWindowLongPtrA : GetWindowLongPtrW;

    pWnd = reinterpret_cast<MyType*>(
        getWindowLongPtr(hWnd, GWLP_USERDATA)
    );

    if (!pWnd) [[unlikely]] {
        throw WND_LAST_EXCEPT();
    }

    if ( !pWnd->nativeHandle() || !pWnd->pHandleMsg_ ) [[unlikely]] {
        constexpr auto& defWindowProc = std::is_same_v<MyChar, CHAR> ?
            DefWindowProcA : DefWindowProcW;
        return defWindowProc(hWnd, msg, wParam, lParam);
    }

    // call the window's message handler
    return ( *(pWnd->pHandleMsg_) )( Message{ msg, wParam, lParam } );
}

template <class Wnd>
LRESULT BasicMsgHandler<Wnd>::operator()(const Message& msg) // overriden
{
    LRESULT result{};

    constexpr auto& defWindProc = std::is_same_v<MyChar, CHAR> ?
        DefWindowProcA : DefWindowProcW;

    try {
        switch (msg.type) {
        case WM_CLOSE:
            PostQuitMessage(0);
            break;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (msg.wParam == VK_SPACE) {
                if constexpr ( std::is_same_v<MyChar, CHAR> ) {
                    window().setTitle("SPACE");
                }
                else /* WCHAR */ {
                    window().setTitle(L"SPACE");
                }
            }
            break;

        default: break;
        }

        result = defWindProc( window().nativeHandle(), msg.type,
            msg.wParam, msg.lParam );

        if (result < 0) [[unlikely]] {
            // error handling
        }

        return result;
    }
    catch (const WindowException& e) {
        MessageBoxA(nullptr, e.what(), "Window Exception",
            MB_OK | MB_ICONEXCLAMATION);
    }
    catch (const std::exception& e) {
        MessageBoxA(nullptr, e.what(), "Standard Exception",
            MB_OK | MB_ICONEXCLAMATION);
    }
    catch(...) {
        MessageBoxA(nullptr, "no details available",
            "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }

    return result;
}

template <Win32Char CharT>
constexpr const BasicWindowTraits<CharT>::MyString
    BasicWindowTraits<CharT>::clsName() noexcept
{
    if constexpr ( std::is_same_v<CharT, CHAR> ) {
        return "WT";
    }
    else /* WCHAR */ {
        return L"WT";
    }
}

template <Win32Char CharT>
constexpr const BasicWindowTraits<CharT>::MyString
    BasicWindowTraits<CharT>::defWndName() noexcept
{
    if constexpr ( std::is_same_v<CharT, CHAR> ) {
        return "Window";
    }
    else /* WCHAR */ {
        return L"Window";
    }
}

template <Win32Char CharT>
constexpr const WndFrame BasicWindowTraits<CharT>::defWndFrame() noexcept
{
    return WndFrame{ .x=200, .y=200, .width=800, .height=600 };
}

template <Win32Char CharT>
void BasicWindowTraits<CharT>::regist(HINSTANCE hInst)
{
    using WndClass = std::conditional_t< std::is_same_v<CharT, CHAR>,
        WNDCLASSEXA, WNDCLASSEXW >;

    WndClass wc = {
        .cbSize = sizeof(WndClass),
        .style = CS_OWNDC,
        .lpfnWndProc = MyWindow::wndProcSetupHandler,
        .cbClsExtra = 0,
        .cbWndExtra = sizeof( LPVOID ),
        .hInstance = hInst,
        .hIcon = nullptr,
        .hCursor = nullptr,
        .hbrBackground = nullptr,
        .lpszMenuName = nullptr,
        .lpszClassName = clsName().data(),
        .hIconSm = nullptr
    };
    
    ATOM bFine{};

    if constexpr ( std::is_same_v<CharT, CHAR> ) {
        bFine = RegisterClassExA(&wc);
    }
    else /* WCHAR */ {
        bFine = RegisterClassExW(&wc);
    }

    if (!bFine) [[unlikely]] {
        throw WND_LAST_EXCEPT();
    }
}

template <Win32Char CharT>
void BasicWindowTraits<CharT>::unregist(HINSTANCE hInst)
{
    bool bFine = false;

    if constexpr ( std::is_same_v<CharT, CHAR> ) {
        bFine = UnregisterClassA( clsName().data(), hInst );
    }
    else /* WCHAR */ {
        bFine = UnregisterClassW( clsName().data(), hInst );
    }

    if (!bFine) [[unlikely]] {
        throw WND_LAST_EXCEPT();
    }
}

template <Win32Char CharT>
HWND BasicWindowTraits<CharT>::create(HINSTANCE hInst, MyWindow* pWnd)
{
    return create( hInst, pWnd, defWndName(), defWndFrame() );
}

template <Win32Char CharT>
HWND BasicWindowTraits<CharT>::create(HINSTANCE hInst, MyWindow* pWnd,
    MyString wndName)
{
    return create( hInst, pWnd, std::move(wndName), defWndFrame() );
}

template <Win32Char CharT>
HWND BasicWindowTraits<CharT>::create(HINSTANCE hInst, MyWindow* pWnd,
    const WndFrame& wndFrame)
{
    return create( hInst, pWnd, defWndName(), wndFrame );
}

template <Win32Char CharT>
HWND BasicWindowTraits<CharT>::create(HINSTANCE hInst, MyWindow* pWnd,
    MyString wndName, const WndFrame& wndFrame)
{
    // additionally store pointer to Window,
    // which makes getting Window object from Win32 window handle possible.

    #define ARG_LISTS   \
        /* .dwExStyle = */ 0, \
        /* .lpClassName = */ clsName().data(),    \
        /* .lpWindowName = */ wndName.data(), \
        /* .dwStyle = */ WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,    \
        /* .X = */ wndFrame.x,    \
        /* .Y = */ wndFrame.y,    \
        /* .nWidth = */ wndFrame.width,   \
        /* .nHeight = */ wndFrame.height, \
        /* .hWndParent = */ nullptr,   \
        /* .hMenu = */ nullptr,    \
        /* .hInstance = */ hInst, \
        /* .lpParam = */ pWnd

    HWND hWnd = nullptr;

    if constexpr ( std::is_same_v<CharT, CHAR> ) {
        hWnd = CreateWindowExA(ARG_LISTS);
    }
    else /* WCHAR */ {
        hWnd = CreateWindowExW(ARG_LISTS);
    }

    if (!hWnd) [[unlikely]] {
        throw WND_LAST_EXCEPT();
    }

    return hWnd;

    #undef ARG_LISTS
}

template <Win32Char CharT>
constexpr const MainWindowTraits<CharT>::MyString
MainWindowTraits<CharT>::clsName() noexcept
{
    if constexpr ( std::is_same_v<CharT, CHAR> ) {
        return "MW";
    }
    else /* WCHAR */ {
        return L"MW";
    }
}

template <Win32Char CharT>
void MainWindowTraits<CharT>::regist(HINSTANCE hInst)
{
    using WndClass = std::conditional_t< std::is_same_v<CharT, CHAR>,
        WNDCLASSEXA, WNDCLASSEXW >;

    WndClass wc = {
        .cbSize = sizeof(WndClass),
        .style = CS_OWNDC,
        .lpfnWndProc = MyWindow::wndProcSetupHandler,
        .cbClsExtra = 0,
        .cbWndExtra = sizeof(LPVOID),
        .hInstance = hInst,
        .hIcon = nullptr,
        .hCursor = nullptr,
        .hbrBackground = nullptr 
    };

    ATOM bFine{};

    if constexpr ( std::is_same_v<CharT, CHAR> ) {
        bFine = RegisterClassExA(&wc);
    }
    else /* WCHAR */ {
        bFine = RegisterClassExW(&wc);
    }

    if (!bFine) [[unlikely]] {
        throw WND_LAST_EXCEPT();
    }
}

template <Win32Char CharT>
void MainWindowTraits<CharT>::unregist(HINSTANCE hInst)
{
    bool bFine = false;

    if constexpr ( std::is_same_v<CharT, CHAR> ) {
        bFine = UnregisterClassA( clsName().data(), hInst );
    }
    else /* WCHAR */ {
        bFine = UnregisterClassW( clsName().data(), hInst );
    }

    if (!bFine) [[unlikely]] {
        throw WND_LAST_EXCEPT();
    }
}

template <Win32Char CharT>
HWND MainWindowTraits<CharT>::create(HINSTANCE hInst, MyWindow* pWnd,
    MyString wndName, const WndFrame& wndFrame)
{
    #define ARG_LISTS   \
        /* .dwExStyle = */ 0, \
        /* .lpClassName = */ clsName().data(),    \
        /* .lpWindowName = */ wndName.data(), \
        /* .dwStyle = */ WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,    \
        /* .X = */ wndFrame.x,    \
        /* .Y = */ wndFrame.y,    \
        /* .nWidth = */ wndFrame.width,   \
        /* .nHeight = */ wndFrame.height, \
        /* .hWndParent = */ nullptr,   \
        /* .hMenu = */ nullptr,    \
        /* .hInstance = */ hInst, \
        /* .lpParam = */ pWnd

    HWND hWnd = nullptr;

    if constexpr ( std::is_same_v<CharT, CHAR> ) {
        hWnd = CreateWindowExA(ARG_LISTS);
    }
    else /* WCHAR */ {
        hWnd = CreateWindowExW(ARG_LISTS);
    }

    if (!hWnd) [[unlikely]] {
        throw WND_LAST_EXCEPT();
    }

    return hWnd;

    #undef ARG_LISTS
}

}   // namespace Win32