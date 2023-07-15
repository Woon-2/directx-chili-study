#include "WindowBasexx.hpp"

#include <type_traits>

template <class CharT>
requires contains<CharT, char, wchar_t>
constexpr const std::basic_string_view<CharT>
WindowTraits<CharT>::clsName() noexcept
{
    if constexpr ( std::is_same_v<CharT, char> ) {
        return "WT";
    }
    else /* wchar_t */ {
        return L"WT";
    }
}

template <class CharT>
requires contains<CharT, char, wchar_t>
constexpr const std::basic_string_view<CharT>
WindowTraits<CharT>::defWndName() noexcept
{
    if constexpr ( std::is_same_v<CharT, char> ) {
        return "Window";
    }
    else /* wchar_t */ {
        return L"Window";
    }
}

template <class CharT>
requires contains<CharT, char, wchar_t>
constexpr const WndFrame WindowTraits<CharT>::defWndFrame() noexcept
{
    return WndFrame{ .x=200, .y=200, .width=800, .height=600 };
}

template <class CharT>
requires contains<CharT, char, wchar_t>
void WindowTraits<CharT>::regist(HINSTANCE hInst)
{
    using WndClass = std::conditional_t< std::is_same_v<CharT, char>,
        WNDCLASSEXA, WNDCLASSEXW >;

    WndClass wc = {
        .cbSize = sizeof(WndClass),
        .style = CS_OWNDC,
        .lpfnWndProc = (std::is_same_v<CharT, char> ?
            DefWindowProcA :
            DefWindowProcW
        ),
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = hInst,
        .hIcon = nullptr,
        .hCursor = nullptr,
        .hbrBackground = nullptr,
        .lpszMenuName = nullptr,
        .lpszClassName = clsName().data(),
        .hIconSm = nullptr
    };
    
    ATOM bFine{};

    if constexpr ( std::is_same_v<CharT, char> ) {
        bFine = RegisterClassExA(&wc);
    }
    else /* wchar_t */ {
        bFine = RegisterClassExW(&wc);
    }

    if (!bFine) {
        MessageBoxA(nullptr, "Fuck", "Woon2 Exception",
            MB_OK | MB_ICONEXCLAMATION);
    }
}

template <class CharT>
requires contains<CharT, char, wchar_t>
void WindowTraits<CharT>::unregist(HINSTANCE hInst)
{
    bool bFine = false;

    if constexpr ( std::is_same_v<CharT, char> ) {
        bFine = UnregisterClassA( clsName().data(), hInst );
    }
    else /* wchar_t */ {
        bFine = UnregisterClassW( clsName().data(), hInst );
    }

    if (!bFine) {
        // error handling
    }
}

template <class CharT>
requires contains<CharT, char, wchar_t>
HWND WindowTraits<CharT>::create(HINSTANCE hInst)
{
    return create( hInst, defWndName(), defWndFrame() );
}

template <class CharT>
requires contains<CharT, char, wchar_t>
HWND WindowTraits<CharT>::create(HINSTANCE hInst, StringLike auto&& wndName)
{
    return create( hInst, wndName, defWndFrame() );
}

template <class CharT>
requires contains<CharT, char, wchar_t>
HWND WindowTraits<CharT>::create(HINSTANCE hInst, const WndFrame& wndFrame)
{
    return create( hInst, defWndName(), wndFrame );
}

template <class CharT>
requires contains<CharT, char, wchar_t>
HWND WindowTraits<CharT>::create(HINSTANCE hInst, StringLike auto&& wndName, const WndFrame& wndFrame)
{
    #define ARG_LISTS   \
        0,  \
        clsName().data(), \
        std::basic_string_view<CharT>(wndName).data(),   \
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,    \
        wndFrame.x,   \
        wndFrame.y,    \
        wndFrame.width,  \
        wndFrame.height, \
        nullptr,    \
        nullptr,    \
        hInst,    \
        nullptr

    HWND hWnd;

    if constexpr ( std::is_same_v<CharT, char> ) {
        hWnd = CreateWindowExA(ARG_LISTS);
    }
    else /* wchar_t */ {
        hWnd = CreateWindowExW(ARG_LISTS);
    }

    if (!hWnd) {
        // error handling
    }

    return hWnd;

    #undef ARG_LISTS
}

template <class CharT>
requires contains<CharT, char, wchar_t>
void WindowTraits<CharT>::show(HWND hWnd)
{
    ShowWindow(hWnd, SW_SHOWDEFAULT);
}

template <class Traits>
Window<Traits>::Window()
{
    if (!bRegist) [[unlikely]] {
        Traits::regist( getHInst() );
        bRegist = true;
    }

    hWnd_ = Traits::create( getHInst() );
    Traits::show(hWnd_);
}

template <class Traits>
template <class ... Args>
Window<Traits>::Window(Args&& ... args)
{
    if (!bRegist) [[unlikely]] {
        Traits::regist( getHInst() );
        bRegist = true;
    }

    hWnd_ = Traits::create( getHInst(), std::forward<Args>(args)... );
    Traits::show(hWnd_);
}

template <class Traits>
void Window<Traits>::setHInst(HINSTANCE hInstance) noexcept
{
    hInst = hInstance;
}

template <class Traits>
HINSTANCE Window<Traits>::getHInst() noexcept
{
    return hInst;
}

template <class Traits>
void Window<Traits>::msgLoop()
{
    MSG msg;
    BOOL result;

    while ( ( result = GetMessageW(&msg, nullptr, 0, 0) ) > 0 ) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}
