#include "WindowBasexx.hpp"

#include <Windows.h>

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

    wnd.msgLoop();

    return 0;
}