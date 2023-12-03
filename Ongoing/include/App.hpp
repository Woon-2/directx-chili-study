#ifndef __APP
#define __APP

#include "ChiliWindow.hpp"
#include "Graphics.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "MsgHandlers.hpp"
#include "Game.hpp"

#include "AdditionalRanges.hpp"

class App {
public:
    using MyWindow = ChiliWindow;
    using MyChar = typename MyWindow::MyChar;
    using MyKeyboard = Keyboard<MyChar>;
    using MyMouse = Mouse;

    App(const Win32::WndFrame& client
        = {.x=200, .y=200, .width=800, .height=600}
    ) : wnd_("ChiliWindow", client),
        gfx_(wnd_), kbd_(), mouse_(), game_(wnd_, gfx_, kbd_, mouse_) {
        setupMsgHandlers();
    }

    [[maybe_unused]] int run() {
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

    static void setHInst(HINSTANCE hInst) {
        ChiliWindow::setHInst(hInst);
    }

    void update() {
        game_.update();
    }

    void render() {
        gfx_.clear(1.f, 1.f, 1.f);
        game_.render();
        gfx_.present();
    }
private: 
    MyWindow& window() noexcept {
        return wnd_;
    }

    const MyWindow& window() const noexcept {
        return wnd_;
    }

    void setupMsgHandlers() {
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
    }

    MyWindow wnd_;
    Graphics<MyWindow> gfx_;
    MyKeyboard kbd_;
    MyMouse mouse_;
    Game game_;
};

#endif  // __APP