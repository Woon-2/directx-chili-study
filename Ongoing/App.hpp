#ifndef __APP
#define __APP

#include "ChiliWindow.hpp"
#include "MsgHandlers.hpp"

#include <memory>

class App {
public:
    App(const Win32::WndFrame& client
        = {.x=200, .y=200, .width=800, .height=600}
    ) : wnd_("ChiliWindow", client) {
        wnd_.msgHandlers().push_front(
            std::make_unique< BasicChiliMsgHandler<ChiliWindow> >(wnd_)
        );
        wnd_.msgHandlers().push_front(
            std::make_unique< KbdMsgHandler<ChiliWindow> >(wnd_)
        );
        wnd_.msgHandlers().push_front(
            std::make_unique< MouseMsgHandler<ChiliWindow> >(wnd_)
        );
    }

    [[maybe_unused]] int run() {
        while(true) {
            if (auto returnCode = wnd_.processMessages()) {
                return returnCode.value();
            }
        }
    }

    static void setHInst(HINSTANCE hInst) {
        ChiliWindow::setHInst(hInst);
    }
private: 
    ChiliWindow wnd_;
};

#endif  // __APP