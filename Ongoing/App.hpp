#ifndef __APP
#define __APP

#include "ChiliWindow.hpp"
#include "MsgHandlers.hpp"
#include "Graphics.hpp"

#include <memory>

class App {
public:
    using MyWindow = ChiliWindow;

    App(const Win32::WndFrame& client
        = {.x=200, .y=200, .width=800, .height=600}
    ) : wnd_("ChiliWindow", client),
        gfx_(wnd_) {
        wnd_.msgHandlers().push_front(
            std::make_unique< BasicChiliMsgHandler<MyWindow> >(wnd_)
        );
        wnd_.msgHandlers().push_front(
            std::make_unique< KbdMsgHandler<MyWindow> >(wnd_)
        );
        wnd_.msgHandlers().push_front(
            std::make_unique< MouseMsgHandler<MyWindow> >(wnd_)
        );
    }

    [[maybe_unused]] int run() {
        while(true) {
            if (auto returnCode = wnd_.processMessages()) {
                return returnCode.value();
            }
            render();
        }
    }

    static void setHInst(HINSTANCE hInst) {
        ChiliWindow::setHInst(hInst);
    }

    void render() {
        gfx_.render();
    }
private: 
    MyWindow wnd_;
    Graphics<MyWindow> gfx_;
};

#endif  // __APP