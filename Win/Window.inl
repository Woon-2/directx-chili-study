#include "Window.hpp"

#include <type_traits>
#include <cassert>
#include <ranges>
#include <algorithm>

namespace Win32
{

template <class Wnd>
std::optional<LRESULT> BasicMsgHandler<Wnd>::
    operator()(const Message& msg) // overriden
{
    std::optional<LRESULT> result{};

    try {
        switch (msg.type) {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;

        default: return {};
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

    return result;
}

template <class Traits>
template <class ... Args>
requires canRegist< Traits, HINSTANCE >
    && canCreate< Traits, HINSTANCE, Window<Traits>*, Args... >
Window<Traits>::Window(Args&& ... args)
    : title_{},
    msgHandlers_{},
    hWnd_{nullptr}
{
    if (!bRegist) [[unlikely]] {
        Traits::regist( getHInst() );
        bRegist = true;
    }

    // enclose this in the Win32 Window,
    // which makes getting Window object from Win32 window handle possible.
    hWnd_ = Traits::create( getHInst(), this, std::forward<Args>(args)... );
    show(SW_SHOWDEFAULT);
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

    if (msg != WM_CREATE) {
        return defWindowProc(hWnd, msg, wParam, lParam);
    }

    using CreateStruct = std::conditional_t< std::is_same_v<MyChar, CHAR>,
        CREATESTRUCTA, CREATESTRUCTW >;

    // extract ptr to window from creation data
    const auto pCreate = reinterpret_cast< const CreateStruct* >( lParam );
    assert(pCreate != nullptr);

    auto pWnd = static_cast< MyType* >( pCreate->lpCreateParams );
    assert(pWnd != nullptr);

    constexpr auto& setWindowLongPtr = std::is_same_v<MyChar, CHAR> ?
        SetWindowLongPtrA : SetWindowLongPtrW;

    // make WinAPI-managed user data to store the ptr to window
    setWindowLongPtr( hWnd, GWLP_USERDATA,
        reinterpret_cast<LONG_PTR>( pWnd )
    );

    // setup is done,
    // substitute message WndProc with regular one.
    setWindowLongPtr( hWnd, GWLP_WNDPROC,
        reinterpret_cast<LONG_PTR>( wndProcCallHandler )
    );

    // as WM_CREATE message is popped in this window procedure,
    // post another WM_CREATE message for Window's message handler
    // to handle WM_CREATE message.

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

    assert(hWnd != nullptr);
    assert(pWnd->nativeHandle() == hWnd);

    // call the window's message handler chain
    // until the message is handled. 
    std::optional<LRESULT> res{};

    auto bHandled = std::ranges::any_of( pWnd->msgHandlers(),
        [&](auto& pHandleMsg) {
            res = (*pHandleMsg)( Message{ msg, wParam, lParam } );
            return res.has_value();
        }
    );
    if (bHandled) {
        return res.value();
    }

    constexpr auto& defWindowProc = std::is_same_v<MyChar, CHAR> ?
        DefWindowProcA : DefWindowProcW;

    return defWindowProc(hWnd, msg, wParam, lParam);
}

template <Win32Char CharT>
void BasicWindowTraits<CharT>::regist(HINSTANCE hInst)
{
    using WndClass = std::conditional_t< std::is_same_v<MyChar, CHAR>,
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

    if constexpr ( std::is_same_v<MyChar, CHAR> ) {
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

    if constexpr ( std::is_same_v<MyChar, CHAR> ) {
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

    if constexpr ( std::is_same_v<MyChar, CHAR> ) {
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