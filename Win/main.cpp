#include <Windows.h>
#include <tchar.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch( msg ) {
    case WM_CLOSE:
        PostQuitMessage(69);
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int CALLBACK WinMain(
    HINSTANCE hInstance,
    [[maybe_unused]] HINSTANCE hPrevInstance,
    [[maybe_unused]] LPSTR lpCmdLine,
    [[maybe_unused]] int nCmdShow
) {
    // Window Creation

    auto className = TEXT("hw3d");
    auto wc = WNDCLASSEX();

    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = static_cast<WNDPROC>(WndProc);
    wc.cbWndExtra = 0;
    wc.cbClsExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = className;
    wc.hIconSm = nullptr;  

    RegisterClassEx(&wc);

    auto hWnd = CreateWindowEx(
        0,
        className,
        TEXT("WinMain"),
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        200, 200, 640, 480, 
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    ShowWindow(hWnd, SW_SHOW);


    // Message Loop

    auto msg = MSG();
    auto result = BOOL();

    while ( ( result = GetMessage( &msg, hWnd, 0, 0 ) ) > 0 ) {
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