#ifndef __Windowxx
#define __Windowxx

#include <Windows.h>

#include <AdditionalConcepts.hpp>
#include <StringLike.hpp>

#include <string_view>

struct WndFrame
{
    int x;
    int y;
    int width;
    int height;
};

template <class CharT>
requires contains<CharT, char, wchar_t>
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

template <class CharT>
requires contains<CharT, char, wchar_t>
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

template <class Traits>
bool Window<Traits>::bRegist = false;

template <class Traits>
HINSTANCE Window<Traits>::hInst = nullptr;

#include "WindowBasexx.inl"

#endif  // __Windowxx