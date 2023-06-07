#include <Windows.h>
#include <tchar.h>
#include <string>
#include <sstream>

#include "Window.hpp"

int CALLBACK WinMain(
    HINSTANCE hInstance,
    [[maybe_unused]] HINSTANCE hPrevInstance,
    [[maybe_unused]] LPSTR lpCmdLine,
    [[maybe_unused]] int nCmdShow
)
{
    Window wnd( 100, 100, 800, 600, TEXT("Sex") );

    MSG msg;
    BOOL result;

    while ( ( result = GetMessage( &msg, wnd.get(), 0, 0 ) ) > 0 ) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (result == -1) {
        return -1;
    }
    else {
        return static_cast<int>(msg.wParam);
    }
}