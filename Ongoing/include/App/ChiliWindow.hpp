#ifndef __ChiliWindow
#define __ChiliWindow

#include "Window.hpp"

#include <string>
#include <string_view>
#include <type_traits>

#include "imgui_impl_win32.h"

#include "Literal.hpp"
#include "Resource.hpp"

namespace Win32 {

struct Client {
    UINT x;
    UINT y;
    UINT width;
    UINT height;
};

template <Win32Char CharT>
struct ChiliWindowTraits {
    using MyWindow = Window<ChiliWindowTraits>;
    using MyChar = CharT;
    using MyStringView = std::basic_string_view<MyChar>;

    static constexpr const MyStringView clsName() noexcept {
        return __LITERAL(MyChar, "ChiliWindowClass");
    }

    static constexpr const MyStringView defWndName() noexcept {
        return __LITERAL(MyChar, "Window");
    }

    static constexpr const WndFrame defWndFrame() noexcept {
        return WndFrame{ .x=200, .y=200, .width=800, .height=600 };
    }

    static void regist(HINSTANCE hInst) {
        using WndClass = std::conditional_t<
            std::is_same_v<MyChar, CHAR>,
            WNDCLASSEXA, WNDCLASSEXW
        >;

        WndClass wc = {
            .cbSize = sizeof(WndClass),
            .style = CS_OWNDC,
            .lpfnWndProc = MyWindow::wndProcSetupHandler,
            .cbClsExtra = 0,
            .cbWndExtra = sizeof(LPVOID),
            .hInstance = hInst,
            .hIcon = static_cast<HICON>(
                LoadImage( hInst,
                    (resourcePath/"icon.ico").c_str(),
                    IMAGE_ICON, 32, 32, LR_LOADFROMFILE
                )
            ),
            .hCursor = nullptr,
            .hbrBackground = nullptr,
            .lpszMenuName = nullptr,
            .lpszClassName = clsName().data(),
            .hIconSm = static_cast<HICON>(
                LoadImage( hInst,
                    (resourcePath/"icon.ico").c_str(),
                    IMAGE_ICON, 16, 16, LR_LOADFROMFILE
                )
            )
        };

        auto bFine = ATOM{};

        if constexpr (std::is_same_v<MyChar, CHAR>) {
            bFine = RegisterClassExA(&wc);
        }
        else /* WCHAR */ {
            bFine = RegisterClassExW(&wc);
        }

        if (!bFine) [[unlikely]] {
            throw WND_LAST_EXCEPT();
        }   
    }

    static void unregist(HINSTANCE hInst)
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

    static void create(HINSTANCE hInst, MyWindow* pWnd) {
        create(hInst, pWnd, defWndFrame());
    }

    static void create(HINSTANCE hInst, MyWindow* pWnd,
        const WndFrame& wndFrame
    ) {
        create(hInst, pWnd, defWndName(), wndFrame);
    }

    static void create(HINSTANCE hInst, MyWindow* pWnd,
        MyStringView wndName
    ) {
        create(hInst, pWnd, wndName, defWndFrame());
    }

    static HWND create( HINSTANCE hInst, MyWindow* pWnd,
        MyStringView wndName, const WndFrame& wndFrame
    ) {
        auto tmp = static_cast<RECT>(wndFrame);

        if ( !AdjustWindowRect(&tmp, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, false) ) {
            throw WND_LAST_EXCEPT();
        }

        #define ARG_LISTS   \
            /* .dwExStyle = */ 0, \
            /* .lpClassName = */ clsName().data(),    \
            /* .lpWindowName = */ wndName.data(), \
            /* .dwStyle = */ WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,    \
            /* .X = */ wndFrame.x,    \
            /* .Y = */ wndFrame.y,    \
            /* .nWidth = */ tmp.right - tmp.left,   \
            /* .nHeight = */ tmp.bottom - tmp.top, \
            /* .hWndParent = */ nullptr,   \
            /* .hMenu = */ nullptr,    \
            /* .hInstance = */ hInst, \
            /* .lpParam = */ pWnd

        auto hWnd = HWND(nullptr);

        if constexpr (std::is_same_v<MyChar, CHAR>) {
            hWnd = CreateWindowExA(ARG_LISTS);
        }
        else /* WCHAR */ {
            hWnd = CreateWindowExW(ARG_LISTS);
        }

        if (!hWnd) {
            throw WND_LAST_EXCEPT();
        }

        return hWnd;

        #undef ARG_LISTS
    }

    static void destroy(HWND hWnd) {
        auto bFine = DestroyWindow(hWnd);

        if (!bFine) {
            throw WND_LAST_EXCEPT();
        }
    }
    static void show(HWND hWnd, int nCmdShow) {
        ShowWindow(hWnd, nCmdShow);
    }

};

template <class Traits>
class BasicChiliWindow : public Win32::Window<Traits>{
public:
    using MyType = BasicChiliWindow<Traits>;
    using MyTraits = Traits;
    using MyChar = Traits::MyChar;
    using MyBase = Win32::Window<Traits>;
    using MyString = std::basic_string<MyChar>;
    using MyStringView = std::basic_string_view<MyChar>;

    using MyBase::nativeHandle;

    BasicChiliWindow() requires false;

    template <class ... Args>
        requires canRegist<Traits, HINSTANCE>
            && canCreate<Traits, HINSTANCE, Window<Traits>*, Args...>
    BasicChiliWindow(Args&& ... args)
        : MyBase( std::forward<Args>(args)... ) {
        ImGui_ImplWin32_Init( nativeHandle() );
    }

    ~BasicChiliWindow() {
        ImGui_ImplWin32_Shutdown();
    }

    BasicChiliWindow(const BasicChiliWindow&) requires false;
    BasicChiliWindow(BasicChiliWindow&&) requires false;
    BasicChiliWindow& operator=(const BasicChiliWindow&) = delete;
    BasicChiliWindow& operator=(BasicChiliWindow&&) = delete;

    const Client client() const {
        RECT tmp;
        GetClientRect( nativeHandle(), &tmp );
        
        return Client{ .x = static_cast<UINT>(tmp.left),
            .y = static_cast<UINT>(tmp.top),
            .width = static_cast<UINT>(tmp.right - tmp.left),
            .height = static_cast<UINT>(tmp.bottom - tmp.top)
        };
    }

private:
};

}   // namespace Win32

using ChiliWindow = Win32::BasicChiliWindow< Win32::ChiliWindowTraits<CHAR> >;
using WChiliWindow = Win32::BasicChiliWindow< Win32::ChiliWindowTraits<WCHAR> >;

#endif  // __ChiliWindow