#include <Windows.h>
#include <tchar.h>
#include <string>
#include <sstream>
#include <memory>
#include <vector>

#include "Window.hpp"

int CALLBACK WinMain(
    HINSTANCE hInstance,
    [[maybe_unused]] HINSTANCE hPrevInstance,
    [[maybe_unused]] LPSTR lpCmdLine,
    [[maybe_unused]] int nCmdShow
)
{
    std::vector<std::unique_ptr<Window>> v;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::wostringstream oss;
            oss << L'(' << j << L", " << i << L")\n";
            v.push_back( std::unique_ptr<Window>( new Window{ 100 + 240 * j, 100 + 180 * i, 240, 180, oss.str() } ) );
        }
    }

    MSG msg;
    BOOL result;

    while ( ( result = GetMessage( &msg, nullptr, 0, 0 ) ) > 0 ) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}