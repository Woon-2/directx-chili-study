#include "App/App.hpp"

#include "App/MsgHandlers.hpp"

#include "UI/ImguiMgr.hpp"

ImguiMgr gImguiMgr;

namespace {
    template <class Wnd, class GFX>
    void initImGui(Wnd&& window, GFX&& gfx) {
        ImGui_ImplWin32_Init(window.nativeHandle());
        ImGui_ImplDX11_Init(
            gfx.factory().device().Get(), gfx.pipeline().context().Get()
        );
    }

    void shutdownImGui() {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
    }

    void beginImGuiFrame() {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void endImGuiFrame() {
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );
    }
}

App::App(const Win32::WndFrame& client)
    : wnd_("ChiliWindow", client),
    gfx_(wnd_), kbd_(), mouse_(), game_(wnd_, gfx_, kbd_, mouse_) {
    initImGui(wnd_, gfx_);
    setupMsgHandlers();
}

App::~App() {
    shutdownImGui();
}

[[maybe_unused]] int App::run() {
    while(true) {
        if (auto returnCode = wnd_.processMessages()) {
            return returnCode.value();
        }

        try {
            update();
            render();
        } catch (const gfx::GraphicsException& e) {
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

void App::render() {
    gfx_.clear(0.f, 0.f, 0.f);
    beginImGuiFrame();
    game_.render();
    endImGuiFrame();
    gfx_.present();
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