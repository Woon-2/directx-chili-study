#ifndef __ChiliMsgHandlers
#define __ChiliMsgHandlers

#include "ChiliWindow.hpp"
#include "Keyboard.hpp"

template <class Wnd>
class BasicChiliMsgHandler : public Win32::MsgHandler<Wnd>{
public:
    using Win32::MsgHandler<Wnd>::window;
    using MyWindow = Wnd;
    using MyChar = typename MyWindow::MyChar;

    BasicChiliMsgHandler(MyWindow& wnd) noexcept
        : Win32::MsgHandler<MyWindow>(wnd) {}

    std::optional<LRESULT> operator()(
        const Win32::Message& msg
    ) override {
        try {
            switch (msg.type) {
            case WM_CLOSE:
                PostQuitMessage(0);
                return 0;

            default:
                break;
            }
        }
        catch (const Win32::WindowException& e) {
            MessageBoxA(nullptr, e.what(), "Window Exception",
                MB_OK | MB_ICONEXCLAMATION);
        }
        catch (const std::exception& e) {
            MessageBoxA(nullptr, e.what(), "Standard Exception",
                MB_OK | MB_ICONEXCLAMATION);
        }
        catch(...) {
            MessageBoxA(nullptr, "no details available",
                "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
        }

        return {};
    }
private:

};

template <class Wnd>
class KbdMsgHandler : public Win32::MsgHandler<Wnd>{
public:
    using Win32::MsgHandler<Wnd>::window;
    using MyWindow = Wnd;
    using MyChar = typename MyWindow::MyChar;
    using MyKeyboard = Keyboard<MyChar>;
    using MyVK = typename MyKeyboard::VK;
    using MyKbdMsgAPI = KeyboardMsgAPI<MyChar>;

    KbdMsgHandler(MyWindow& wnd) noexcept
        : Win32::MsgHandler<MyWindow>(wnd), kbd_() {}

    std::optional<LRESULT> operator()(
        const Win32::Message& msg
    ) override {
        try {
            switch (msg.type) {
            // clear scanned key state when window loses focus
            case WM_KILLFOCUS:
                MyKbdMsgAPI::clearScanned(kbd_);
                break;

            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
                MyKbdMsgAPI::onKeyPressed(
                    kbd_, static_cast<MyVK>(msg.wParam)
                );
                break;

            case WM_KEYUP:
            case WM_SYSKEYUP:
                MyKbdMsgAPI::onKeyReleased(
                    kbd_, static_cast<MyVK>(msg.wParam)
                );
                break;

            case WM_CHAR:
                MyKbdMsgAPI::onChar(
                    kbd_, static_cast<MyChar>(msg.wParam)
                );
                break;

            default:
                break;
            }
        }
        catch (const Win32::WindowException& e) {
            MessageBoxA(nullptr, e.what(), "Window Exception",
                MB_OK | MB_ICONEXCLAMATION);
        }
        catch (const std::exception& e) {
            MessageBoxA(nullptr, e.what(), "Standard Exception",
                MB_OK | MB_ICONEXCLAMATION);
        }
        catch(...) {
            MessageBoxA(nullptr, "no details available",
                "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
        }

        if ( auto ev = kbd_.readKey() ) {
            if ( ev->pressed() && ev->keycode() == VK_SPACE ) {
                MessageBoxA(nullptr, "Space Key Pressed!", "Key Press",
                    MB_OK | MB_ICONEXCLAMATION);
            }
        }

        return {};
    }
private:
    MyKeyboard kbd_;
};

#endif // __ChiliMsgHandlers