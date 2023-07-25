#include "Window.hpp"

#include <Windows.h>

namespace Win32
{

template <Win32Char CharT>
struct MainWindowTraits
{
    using MyWindow = Window< MainWindowTraits >;
    using MyChar = CharT;
    using MyString = std::basic_string_view<MyChar>;

    static constexpr const MyString
        clsName() noexcept;
    static void regist(HINSTANCE hInst);
    static void unregist(HINSTANCE hInst);
    static HWND create(HINSTANCE hInst, MyWindow* pWnd,
        MyString wndName, const WndFrame& wndFrame);
    static void destroy(HWND hWnd)
    {
        auto bFine = DestroyWindow(hWnd);

        if (!bFine) {
            throw WND_LAST_EXCEPT();
        }
    }
    static void show(HWND hWnd, int nCmdShow) { ShowWindow(hWnd, nCmdShow); }
};

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

using MyWindow = Win32::Window< Win32::BasicWindowTraits<WCHAR> >;

int CALLBACK WinMain(
    HINSTANCE hInst,
    [[maybe_unused]] HINSTANCE hPrevInst,
    [[maybe_unused]] LPSTR lpCmdLine,
    [[maybe_unused]] int nCmdShow
)
{
    MyWindow::setHInst(hInst);
    MyWindow wnd;
    wnd.msgHandlers().push_back(
        std::make_unique< Win32::BasicMsgHandler<MyWindow> >(wnd) 
    );

    wnd.msgLoop();

    return 0;
}