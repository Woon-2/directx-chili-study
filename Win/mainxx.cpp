#include "WindowBasexx.hpp"

#include <Windows.h>

int CALLBACK WinMain(
    HINSTANCE hInst,
    [[maybe_unused]] HINSTANCE hPrevInst,
    [[maybe_unused]] LPSTR lpCmdLine,
    [[maybe_unused]] int nCmdShow
)
{
    WindowTraits<wchar_t>::regist(hInst);
    auto hWnd = WindowTraits<wchar_t>::create(hInst);
    WindowTraits<wchar_t>::show(hWnd);

    MSG msg;
    BOOL result;

    while ( ( result = GetMessageW(&msg, nullptr, 0, 0) ) > 0 ) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}