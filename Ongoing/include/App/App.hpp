#ifndef __APP
#define __APP

#include "ChiliWindow.hpp"
#include "GFX/Core/Graphics.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "Game/Game.hpp"

class App {
public:
    using MyWindow = ChiliWindow;
    using MyChar = typename MyWindow::MyChar;
    using MyKeyboard = Keyboard<MyChar>;
    using MyMouse = Mouse;

    App(const Win32::WndFrame& frame
        = { .x=200, .y=200, .width=1024, .height=780 }
    );

    ~App();

    [[maybe_unused]] int run();

    static void setHInst(HINSTANCE hInst) {
        ChiliWindow::setHInst(hInst);
    }

    void update() {
        game_.update();
    }

    void render();
private: 
    MyWindow& window() noexcept {
        return wnd_;
    }

    const MyWindow& window() const noexcept {
        return wnd_;
    }

    void setupMsgHandlers();

    MyWindow wnd_;
    gfx::Graphics gfx_;
    MyKeyboard kbd_;
    MyMouse mouse_;
    Game game_;
};

#endif  // __APP