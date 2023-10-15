#ifndef __APP
#define __APP

#include "ChiliWindow.hpp"
#include "MsgHandlers.hpp"
#include "Graphics.hpp"
#include "Timer.hpp"

#include <memory>

#include <cmath>

class App {
public:
    using MyWindow = ChiliWindow;
    using MyTimer = Timer<float>;

    App(const Win32::WndFrame& client
        = {.x=200, .y=200, .width=800, .height=600}
    ) : wnd_("ChiliWindow", client),
        gfx_(wnd_), timer_() {
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
        try {
            const float c = sin( timer_.peek() ) / 2.f + 0.5f;
            gfx_.clear(c,c,1.f);
            gfx_.render(timer_.peek());
        } catch (const GraphicsException& e) {
            MessageBoxA(nullptr, e.what(), "Graphics Exception",
                MB_OK | MB_ICONEXCLAMATION);
            wnd_.close();
        }
        catch (const Win32::WindowException& e) {
            MessageBoxA(nullptr, e.what(), "Window Exception",
                MB_OK | MB_ICONEXCLAMATION);
            wnd_.close();
        }
        catch (const std::exception& e) {
            MessageBoxA(nullptr, e.what(), "Standard Exception",
                MB_OK | MB_ICONEXCLAMATION);
            wnd_.close();
        }
        catch(...) {
            MessageBoxA(nullptr, "no details available",
                "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
            wnd_.close();
        }
    }
private: 
    MyWindow wnd_;
    Graphics<MyWindow> gfx_;
    MyTimer timer_;
};

#endif  // __APP