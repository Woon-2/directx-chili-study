#include <string>
#include <string_view>
#include <memory>

#include "ChiliWindow.hpp"

int CALLBACK WinMain(
    HINSTANCE hInst,
    [[maybe_unused]] HINSTANCE hPrevInst,
    [[maybe_unused]] LPSTR lpCmdLine,
    [[maybe_unused]] int nCmdShow
) {
    ChiliWindow::setHInst(hInst);

    auto wnd = ChiliWindow("ChiliWindow", Win32::WndFrame{
        .x=200, .y=200, .width=800, .height=600
    });

    wnd.msgHandlers().push_back(
        std::make_unique< Win32::BasicMsgHandler<ChiliWindow> >(wnd)
    );

    wnd.msgLoop();

    return 0;
}