#ifndef __ChiliMsgHandlers
#define __ChiliMsgHandlers

#include "ChiliWindow.hpp"

template <class Wnd>
class BasicChiliMsgHandler : public Win32::MsgHandler<Wnd>{
public:
    using Win32::MsgHandler<Wnd>::window;
    using MyChar = typename Win32::MsgHandler<Wnd>::MyChar;

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

#endif // __ChiliMsgHandlers