#include "App/App.hpp"

#include "App/MsgHandlers.hpp"

App::App(const Win32::WndFrame& client)
    : wnd_("ChiliWindow", client),
    gfx_(wnd_), kbd_(), mouse_(), game_(wnd_, gfx_, kbd_, mouse_) {
    setupMsgHandlers();
}

[[maybe_unused]] int App::run() {
    while(true) {
        if (auto returnCode = wnd_.processMessages()) {
            return returnCode.value();
        }

        try {
            update();
            render();
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
}

void App::setupMsgHandlers() {
    window().msgHandlers().push_front(
        std::make_unique< BasicChiliMsgHandler<MyWindow> >(
            window()
        )
    );
    window().msgHandlers().push_front(
        std::make_unique < KbdMsgHandler<MyWindow> >(
            window(), &kbd_
        )
    );
    window().msgHandlers().push_front(
        std::make_unique< MouseMsgHandler<MyWindow> >(
            window(), &mouse_
        )
    );
    window().msgHandlers().push_front(
        std::make_unique< ImguiMsgHandler<MyWindow> >(
            window()
        )
    );
}