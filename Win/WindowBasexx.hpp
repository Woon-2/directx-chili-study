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
struct WindowTraits
{
    static constexpr const std::basic_string_view<CharT> clsName() noexcept;
    static constexpr const std::basic_string_view<CharT> defWndName() noexcept;
    static constexpr const WndFrame defWndFrame() noexcept;
    static void regist(HINSTANCE hInst);
    static HWND create(HINSTANCE hInst);
    static HWND create(HINSTANCE hInst, StringLike auto&& wndName);
    static HWND create(HINSTANCE hInst, const WndFrame& wndFrame);
    static HWND create(HINSTANCE hInst, StringLike auto&& wndName, const WndFrame& wndFrame);
    static void show(HWND hWnd);
};

template <class Traits>
class Window
{
public:

private:
};

#include "WindowBasexx.inl"

#endif  // __Windowxx