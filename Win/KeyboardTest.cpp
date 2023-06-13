#include "KeyboardTestWindow.hpp"

#include <fstream>

int CALLBACK WinMain(
    HINSTANCE hInstance,
    [[maybe_unused]] HINSTANCE hPrevInstance,
    [[maybe_unused]] LPSTR lpCmdLine,
    [[maybe_unused]] int nCmdShow
)
{
    try {
        KeyboardTestWindow wnd(RECT{200, 200, 1000, 800});
        std::ofstream out("KeyboardTestOutput.txt");
        std::ofstream outScan("KeyboardScanned.txt");

        MSG msg;
        BOOL result;

        while ((result = GetMessage(&msg, nullptr, 0, 0)) > 0) {
            const auto& testOutput = wnd.getTestOutput();

            for (auto ch : testOutput) {
                out.put(static_cast<char>(ch));
            }
            wnd.clearTestOutput();

            wnd.scan();

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        out.close();
        
        for (auto ch : wnd.getScanned()) {
            outScan.put(static_cast<char>(ch));
        }
    }   // try block
    catch (const Woon2Exception& e) {
        MessageBoxA(nullptr, e.what(), "Woon2 Exception",
            MB_OK | MB_ICONEXCLAMATION);
    }
    catch (const std::exception& e) {
        MessageBoxA(nullptr, e.what(), "Standard Exception",
            MB_OK | MB_ICONEXCLAMATION);
    }
    catch (...) {
        MessageBoxA(nullptr, "no details available",
            "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }

    return 0;
}