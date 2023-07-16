#include "WindowBasexx.hpp"

#include <type_traits>

template <class CharT>
requires contains<CharT, char, wchar_t>
constexpr const std::basic_string_view<CharT>
BasicWindowTraits<CharT>::clsName() noexcept
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
BasicWindowTraits<CharT>::defWndName() noexcept
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
constexpr const WndFrame BasicWindowTraits<CharT>::defWndFrame() noexcept
{
    return WndFrame{ .x=200, .y=200, .width=800, .height=600 };
}

template <class CharT>
requires contains<CharT, char, wchar_t>
void BasicWindowTraits<CharT>::regist(HINSTANCE hInst)
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
        throw WND_LAST_EXCEPT();
    }
}

template <class CharT>
requires contains<CharT, char, wchar_t>
void BasicWindowTraits<CharT>::unregist(HINSTANCE hInst)
{
    bool bFine = false;

    if constexpr ( std::is_same_v<CharT, char> ) {
        bFine = UnregisterClassA( clsName().data(), hInst );
    }
    else /* wchar_t */ {
        bFine = UnregisterClassW( clsName().data(), hInst );
    }

    if (!bFine) {
        throw WND_LAST_EXCEPT();
    }
}

template <class CharT>
requires contains<CharT, char, wchar_t>
HWND BasicWindowTraits<CharT>::create(HINSTANCE hInst)
{
    return create( hInst, defWndName(), defWndFrame() );
}

template <class CharT>
requires contains<CharT, char, wchar_t>
HWND BasicWindowTraits<CharT>::create(HINSTANCE hInst,
    std::basic_string_view<CharT> wndName)
{
    return create( hInst, wndName, defWndFrame() );
}

template <class CharT>
requires contains<CharT, char, wchar_t>
HWND BasicWindowTraits<CharT>::create(HINSTANCE hInst, const WndFrame& wndFrame)
{
    return create( hInst, defWndName(), wndFrame );
}

template <class CharT>
requires contains<CharT, char, wchar_t>
HWND BasicWindowTraits<CharT>::create(HINSTANCE hInst,
    std::basic_string_view<CharT> wndName, const WndFrame& wndFrame)
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
        /* .lpParam = */ nullptr

    HWND hWnd = nullptr;

    if constexpr ( std::is_same_v<CharT, char> ) {
        hWnd = CreateWindowExA(ARG_LISTS);
    }
    else /* wchar_t */ {
        hWnd = CreateWindowExW(ARG_LISTS);
    }

    if (!hWnd) {
        throw WND_LAST_EXCEPT();
    }

    return hWnd;

    #undef ARG_LISTS
}

template <class CharT>
requires contains<CharT, char, wchar_t>
void BasicWindowTraits<CharT>::show(HWND hWnd)
{
    ShowWindow(hWnd, SW_SHOWDEFAULT);
}

template <class CharT>
requires contains<CharT, char, wchar_t>
constexpr const std::basic_string_view<CharT>
MainWindowTraits<CharT>::clsName() noexcept
{
    if constexpr ( std::is_same_v<CharT, char> ) {
        return "MW";
    }
    else /* wchar_t */ {
        return L"MW";
    }
}

template <class CharT>
requires contains<CharT, char, wchar_t>
void MainWindowTraits<CharT>::regist(HINSTANCE hInst, WNDPROC wndProc)
{
    using WndClass = std::conditional_t< std::is_same_v<CharT, char>,
        WNDCLASSEXA, WNDCLASSEXW >;

    WndClass wc = {
        .cbSize = sizeof(WndClass),
        .style = CS_OWNDC,
        .lpfnWndProc = wndProc,
        .cbClsExtra = 0,
        .cbWndExtra = sizeof(LPVOID),
        .hInstance = hInst,
        .hIcon = nullptr,
        .hCursor = nullptr,
        .hbrBackground = nullptr 
    };

    ATOM bFine{};

    if constexpr ( std::is_same_v<CharT, char> ) {
        bFine = RegisterClassExA(&wc);
    }
    else /* wchar_t */ {
        bFine = RegisterClassExW(&wc);
    }

    if (!bFine) {
        throw WND_LAST_EXCEPT();
    }
}

template <class CharT>
requires contains<CharT, char, wchar_t>
void MainWindowTraits<CharT>::unregist(HINSTANCE hInst)
{
    bool bFine = false;

    if constexpr ( std::is_same_v<CharT, char> ) {
        bFine = UnregisterClassA( clsName().data(), hInst );
    }
    else /* wchar_t */ {
        bFine = UnregisterClassW( clsName().data(), hInst );
    }

    if (!bFine) {
        throw WND_LAST_EXCEPT();
    }
}

template <class CharT>
requires contains<CharT, char, wchar_t>
HWND MainWindowTraits<CharT>::create(HINSTANCE hInst,
    std::basic_string_view<CharT> wndName, const WndFrame& wndFrame,
    LPVOID lpParam)
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
        /* .lpParam = */ lpParam

    HWND hWnd = nullptr;

    if constexpr ( std::is_same_v<CharT, char> ) {
        hWnd = CreateWindowExA(ARG_LISTS);
    }
    else /* wchar_t */ {
        hWnd = CreateWindowExW(ARG_LISTS);
    }

    if (!hWnd) {
        throw WND_LAST_EXCEPT();
    }

    return hWnd;

    #undef ARG_LISTS
}

template <class CharT>
requires contains<CharT, char, wchar_t>
void MainWindowTraits<CharT>::show(HWND hWnd)
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
Window<Traits>::~Window()
{
    DestroyWindow(hWnd_);
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
