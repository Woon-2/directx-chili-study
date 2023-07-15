#include "WindowBasexx.hpp"

#include <Windows.h>

int CALLBACK WinMain(
    HINSTANCE hInst,
    [[maybe_unused]] HINSTANCE hPrevInst,
    [[maybe_unused]] LPSTR lpCmdLine,
    [[maybe_unused]] int nCmdShow
)
{
    Window< WindowTraits<wchar_t> >::setHInst(hInst);
    Window< WindowTraits<wchar_t> > wnd;

    wnd.msgLoop();

    return 0;
}