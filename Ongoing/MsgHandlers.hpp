#ifndef __ChiliMsgHandlers
#define __ChiliMsgHandlers

#include "ChiliWindow.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"

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
    using MyString = std::basic_string<MyChar>;

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
                if ( !(msg.lParam & 0x40000000) || kbd_.autoRepeat() ) {
                    MyKbdMsgAPI::onKeyPressed(
                        kbd_, static_cast<MyVK>(msg.wParam)
                    );
                }
                return 0;

            case WM_KEYUP:
            case WM_SYSKEYUP:
                MyKbdMsgAPI::onKeyReleased(
                    kbd_, static_cast<MyVK>(msg.wParam)
                );
                return 0;

            case WM_CHAR:
                MyKbdMsgAPI::onChar(
                    kbd_, static_cast<MyChar>(msg.wParam)
                );
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
    MyKeyboard kbd_;
};

#include <sstream>

template <class Wnd>
class MouseMsgHandler : public Win32::MsgHandler<Wnd>{
public:
    using Win32::MsgHandler<Wnd>::window;
    using MyWindow = Wnd;
    using MyChar = typename MyWindow::MyChar;
    using MyMouse = Mouse;
    using MyMouseMsgAPI = MouseMsgAPI;
    using MyString = std::basic_string<MyChar>;

    MouseMsgHandler(MyWindow& wnd) noexcept
        : Win32::MsgHandler<MyWindow>(wnd), mouse_() {}

    std::optional<LRESULT> operator()(
        const Win32::Message& msg
    ) override {
        try {
            while (!mouse_.empty()) {
                const auto e = mouse_.read();
                std::ostringstream oss;

                switch(e->type().value()) {
                case Mouse::Event::Type::Move:
                    oss << "Mouse Position: (" << e->pos().x << ", " << e->pos().y << ")";
                    window().setTitle(oss.str());
                    break;

                case Mouse::Event::Type::LPress:
                    window().setTitle("LPress");
                    break;

                case Mouse::Event::Type::LRelease:
                    window().setTitle("LRelease");
                    break;

                case Mouse::Event::Type::MPress:
                    window().setTitle("MPress");
                    break;

                case Mouse::Event::Type::MRelease:
                    window().setTitle("MRelease");
                    break;

                case Mouse::Event::Type::RPress:
                    window().setTitle("RPress");
                    break;

                case Mouse::Event::Type::RRelease:
                    window().setTitle("RRelease");
                    break;

                case Mouse::Event::Type::WheelUp:
                    window().setTitle("WheelUp");
                    break;

                case Mouse::Event::Type::WheelDown:
                    window().setTitle("WheelDown");
                    break;

                default:
                    break;
                }
            }

            auto pt = makePoint(msg.lParam);

            switch (msg.type) {
            case WM_MOUSEMOVE:
                if ( insideClient(pt) ) {
                    MyMouseMsgAPI::onMouseMove(mouse_, pt);

                    if ( !mouse_.inWindow() ) {
                        SetCapture( window().nativeHandle() );
                        MyMouseMsgAPI::onEnter(mouse_, pt);
                    }
                }
                else {
                    if ( msg.wParam & (MK_LBUTTON | MK_RBUTTON) ) {
                        MyMouseMsgAPI::onMouseMove(mouse_, pt);
                    }
                    else {
                        ReleaseCapture();
                        MyMouseMsgAPI::onLeave(mouse_, pt);
                    }
                }
                return 0;

            case WM_LBUTTONDOWN:
                MyMouseMsgAPI::onLeftPressed(mouse_, pt);
                return 0;

            case WM_LBUTTONUP:
                MyMouseMsgAPI::onLeftReleased(mouse_, pt);
                return 0;

            case WM_RBUTTONDOWN:
                MyMouseMsgAPI::onRightPressed(mouse_, pt);
                return 0;

            case WM_RBUTTONUP:
                MyMouseMsgAPI::onRightReleased(mouse_, pt);
                return 0;

            case WM_MBUTTONDOWN:
                MyMouseMsgAPI::onMidPressed(mouse_, pt);
                return 0;

            case WM_MBUTTONUP:
                MyMouseMsgAPI::onMidReleased(mouse_, pt);
                return 0;

            case WM_MOUSEWHEEL:
                MyMouseMsgAPI::onMouseWheel(mouse_, pt,
                    GET_WHEEL_DELTA_WPARAM(msg.wParam)
                );
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
    static Mouse::Point makePoint(LPARAM lParam) {
        return Mouse::Point{
            static_cast<short>(
                (static_cast<unsigned int>(lParam)) & 0xffff
            ),
            static_cast<short>(
                (static_cast<unsigned int>(lParam) >> 16) & 0xffff
            )
        };
    }

    bool insideClient(Mouse::Point pt) noexcept {
        auto client = window().client();

        return pt.x >= client.x && pt.y >= client.y
            && pt.x <= client.x + client.width
            && pt.y <= client.y + client.height;
    }

    MyMouse mouse_;
};

#endif // __ChiliMsgHandlers