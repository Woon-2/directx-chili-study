#ifndef __APP
#define __APP

#include "Scene.hpp"
#include "Entity.hpp"
#include "Renderer.hpp"

#include "Box.hpp"

#include "ChiliWindow.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "MsgHandlers.hpp"
#include "Graphics.hpp"
#include "GraphicsStorage.hpp"

#include "Timer.hpp"

#include <memory>
#include <vector>

#include <cmath>

#include "AdditionalRanges.hpp"

class App {
public:
    using MyWindow = ChiliWindow;
    using MyChar = typename MyWindow::MyChar;
    using MyTimer = Timer<float>;
    using MyKeyboard = Keyboard<MyChar>;
    using MyMouse = Mouse;

    App(const Win32::WndFrame& client
        = {.x=200, .y=200, .width=800, .height=600}
    ) : wnd_("ChiliWindow", client),
        gfx_(wnd_), kbd_(), mouse_(), timer_(), scene_(),
        renderer_(), entities_() {
        wnd_.setClient(client);
        setupMsgHandlers();
        loadGFXResources();
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
            static auto mousePos = MyMouse::Point{};
            if (auto ev = mouse_.peek()) {
                mousePos = ev->pos();
            }

            const float c = sin( timer_.peek() ) / 2.f + 0.5f;
            gfx_.clear(c,c,1.f);
            renderer_.render(scene_);
            gfx_.render(
                timer_.peek(),
                mousePos.x / static_cast<float>(wnd_.client().width) * 2 - 1.f,
                mousePos.y / static_cast<float>(wnd_.client().height) * -2 + 1.f
            );
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

    void loadGFXResources() {
        renderer_.linkPipeline( gfx_.pipeline() );

        entities_.push_back( std::make_unique<Entity<Box>>( Box{} ));

        std::ranges::for_each( entities_ | dereference(), [this](auto& entity) {
            entity.loadAt(gfx_.pDevice(), scene_);
        });

        scene_.sortFor(renderer_);
    }

    MyWindow wnd_;
    Graphics<MyWindow> gfx_;
    MyKeyboard kbd_;
    MyMouse mouse_;
    MyTimer timer_;
    
    Scene scene_;
    Renderer renderer_;
    std::vector< std::unique_ptr<IEntity> > entities_;
};

#endif  // __APP