#include "Window.hpp"

#include <Windows.h>

using MyWindow = Win32::Window< Win32::MainWindowTraits<WCHAR> >;

int CALLBACK WinMain(
    HINSTANCE hInst,
    [[maybe_unused]] HINSTANCE hPrevInst,
    [[maybe_unused]] LPSTR lpCmdLine,
    [[maybe_unused]] int nCmdShow
)
{
    MyWindow::setHInst(hInst);
    MyWindow wnd(L"Fuck", Win32::WndFrame{200, 200, 400, 300});

    wnd.msgLoop();

    return 0;
}