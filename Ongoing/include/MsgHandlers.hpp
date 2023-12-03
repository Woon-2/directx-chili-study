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
                if constexpr (std::is_same_v<MyChar, CHAR>) {
                    auto ans = MessageBoxA(window().nativeHandle(),
                        "Will you really close the application?",
                        window().title().data(), MB_YESNO
                    );
                    
                    if (ans == IDYES) {
                        PostMessageA(window().nativeHandle(),
                            WM_DESTROY, 0, 0
                        );
                    }
                }
                else /* WCHAR */ {
                    auto ans = MessageBoxW(window().nativeHandle(),
                        L"Will you really close the application?",
                        window().title().data(), MB_YESNO
                    );
                    
                    if (ans == IDYES) {
                        PostMessageW(window().nativeHandle(),
                            WM_DESTROY, 0, 0
                        );
                    }
                }
                return 0;

            case WM_DESTROY:
                if constexpr (std::is_same_v<MyChar, CHAR>) {
                    PostMessageA(window().nativeHandle(),
                        WM_QUIT, 0, 0
                    );
                }
                else /* WCHAR */ {
                    PostMessageW(window().nativeHandle(),
                        WM_QUIT, 0, 0
                    );
                }
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

    KbdMsgHandler(MyWindow& wnd, MyKeyboard* pKbd) noexcept
        : Win32::MsgHandler<MyWindow>(wnd), pKbd_(pKbd) {}

    std::optional<LRESULT> operator()(
        const Win32::Message& msg
    ) override {
        try {
            switch (msg.type) {
            // clear scanned key state when window loses focus
            case WM_KILLFOCUS:
                MyKbdMsgAPI::clearScanned(*pKbd_);
                break;

            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
                if ( !(msg.lParam & 0x40000000) || pKbd_->autoRepeat() ) {
                    MyKbdMsgAPI::onKeyPressed(
                        *pKbd_, static_cast<MyVK>(msg.wParam)
                    );
                }
                return 0;

            case WM_KEYUP:
            case WM_SYSKEYUP:
                MyKbdMsgAPI::onKeyReleased(
                    *pKbd_, static_cast<MyVK>(msg.wParam)
                );
                return 0;

            case WM_CHAR:
                MyKbdMsgAPI::onChar(
                    *pKbd_, static_cast<MyChar>(msg.wParam)
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
    MyKeyboard* pKbd_;
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

    MouseMsgHandler(MyWindow& wnd, MyMouse* pMouse) noexcept
        : Win32::MsgHandler<MyWindow>(wnd), pMouse_(pMouse) {}

    std::optional<LRESULT> operator()(
        const Win32::Message& msg
    ) override {
        try {
            while (!pMouse_->empty()) {
                const auto e = pMouse_->read();
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
                    MyMouseMsgAPI::onMouseMove(*pMouse_, pt);

                    if ( !pMouse_->inWindow() ) {
                        SetCapture( window().nativeHandle() );
                        MyMouseMsgAPI::onEnter(*pMouse_, pt);
                    }
                }
                else {
                    if ( msg.wParam & (MK_LBUTTON | MK_RBUTTON) ) {
                        MyMouseMsgAPI::onMouseMove(*pMouse_, pt);
                    }
                    else {
                        ReleaseCapture();
                        MyMouseMsgAPI::onLeave(*pMouse_, pt);
                    }
                }
                return 0;

            case WM_LBUTTONDOWN:
                MyMouseMsgAPI::onLeftPressed(*pMouse_, pt);
                return 0;

            case WM_LBUTTONUP:
                MyMouseMsgAPI::onLeftReleased(*pMouse_, pt);
                return 0;

            case WM_RBUTTONDOWN:
                MyMouseMsgAPI::onRightPressed(*pMouse_, pt);
                return 0;

            case WM_RBUTTONUP:
                MyMouseMsgAPI::onRightReleased(*pMouse_, pt);
                return 0;

            case WM_MBUTTONDOWN:
                MyMouseMsgAPI::onMidPressed(*pMouse_, pt);
                return 0;

            case WM_MBUTTONUP:
                MyMouseMsgAPI::onMidReleased(*pMouse_, pt);
                return 0;

            case WM_MOUSEWHEEL:
                MyMouseMsgAPI::onMouseWheel(*pMouse_, pt,
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

        auto cx = static_cast<decltype(pt.x)>(client.x);
        auto cy = static_cast<decltype(pt.y)>(client.y);
        auto cw = static_cast<decltype(pt.x)>(client.width);
        auto ch = static_cast<decltype(pt.y)>(client.height);

        return pt.x >= cx && pt.y >= cy
            && pt.x <= cx + cw
            && pt.y <= cy + ch;
    }

    MyMouse* pMouse_;
};

#endif // __ChiliMsgHandlers