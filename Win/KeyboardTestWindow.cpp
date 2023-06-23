#include "KeyboardTestWindow.hpp"

#include <vector>
#include <sstream>

#include <Quote.hpp>
#include <Bit.hpp>

#define ENABLE_KEY_LIST 'W', 'w', 'A', 'a', 'S', 's', 'D', 'd', VK_SPACE

std::unique_ptr< KeyboardTestWindow::WindowClass >
KeyboardTestWindow::pWndClass;

KeyboardTestWindow::WindowClass::WindowClass()
    : hInst_( GetModuleHandleW(nullptr) ), name_(L"TestWindowClass")
{
    if ( !hInst_ ) {
        // on GetMoudleHandle failed
        throw WND_LAST_EXCEPT();
    }

    WNDCLASSEXW wc;

    // fill wc
    wc.cbSize = sizeof(wc);
    wc.cbWndExtra = 0;
    wc.cbClsExtra = 0;
    wc.hbrBackground = nullptr;
    wc.hCursor = nullptr;
    wc.hIcon = nullptr;
    wc.hIcon = static_cast<HICON>(LoadImageW(
        nullptr,
        QUOTEW(RESOURCE_PATH/icon.ico),
        IMAGE_ICON,
        0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE
    ));
    if (!wc.hIcon) {
        // on icon setting failed
        throw WND_LAST_EXCEPT();
    }
    wc.hIconSm = nullptr;
    wc.hInstance = hInst_;
    wc.lpszClassName = name_.c_str();
    wc.style = CS_OWNDC;
    wc.lpszMenuName = nullptr;

    // set WndProc in base class
    injectWndProc(wc);

    // register wc
    if (!RegisterClassExW(&wc)) {
        // on RegisterClassEx failed
        throw WND_LAST_EXCEPT();
    }

}

KeyboardTestWindow::WindowClass::~WindowClass()
{
    UnregisterClassW(name_.c_str(), hInst_);
}

HINSTANCE KeyboardTestWindow::WindowClass::getInst() const
{
    return hInst_;
}

const std::wstring& KeyboardTestWindow::WindowClass::getName() const
{
    return name_;
}

KeyboardTestWindow::KeyboardTestWindow(const RECT& region)
    : region_(region), hWnd_(nullptr),
    kbd( std::set< Keyboard::KeyType >( {ENABLE_KEY_LIST} ) ),
    testOutput_(), pressScanned_(), releaseScanned_()
{
    if ( !pWndClass ) {
        // lazy initialization of window class
        initWindowClass();
    }
    /*
     Client area's size is slightly smaller than whole window area,
     which is caused by the window outline and title bar.
     So adjust the client area's size up to the intended size.
    */
    if ( !AdjustWindowRect( &region_, WS_CAPTION | WS_MINIMIZEBOX
        | WS_SYSMENU, false ) ) {
        // on AdjustWindowRect failed
        throw WND_LAST_EXCEPT();
    }

    hWnd_ = CreateWindowExW(
        0,
        pWndClass->getName().c_str(),
        L"TestWindow",
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        region_.left,
        region_.top,
        region_.right - region_.left,
        region_.bottom - region_.top,
        nullptr,
        nullptr,
        pWndClass->getInst(),
        this
    );

    if (hWnd_ == nullptr) {
        // on window creation failed
        throw WND_LAST_EXCEPT();
    }

    ShowWindow(hWnd_, SW_SHOWDEFAULT);
}

KeyboardTestWindow::~KeyboardTestWindow()
{
    DestroyWindow(hWnd_);
}

const std::wstring& KeyboardTestWindow::getTestOutput() const
{
    return testOutput_;
}

void KeyboardTestWindow::clearTestOutput()
{
    testOutput_.clear();
}

void KeyboardTestWindow::outputKeyboardInput()
{
    while ( auto ev = kbd.getEventQueue().readKey() ) {
        std::wostringstream oss;
        oss << static_cast<char>( ev->getKey() ) << L' ';
        if (ev->getState() == Keyboard::KeyState::Pressed) {
            oss << L"Pressed\n";
        }
        else if (ev->getState() == Keyboard::KeyState::Released) {
            oss << L"Released\n";
        }
        else /* if (ev->getState() == Keyboard::KeyState::Invalid) */ {
            oss << L"Invalid\n";
        }

        testOutput_ += oss.str();
    }

    while (auto ch = kbd.getEventQueue().readChar())
    {
        testOutput_ += static_cast<wchar_t>( *ch );
        testOutput_ += L'\n';
    }
}

std::wstring KeyboardTestWindow::getScanned() const
{
    return std::wstring(L"Press: ") + pressScanned_ + std::wstring(L"\nRelease: ")
        + releaseScanned_;
}

void KeyboardTestWindow::clearScanned()
{
    pressScanned_.clear();
    releaseScanned_.clear();
}

void KeyboardTestWindow::scan()
{
    kbd.scan();
    for (auto ch : std::vector<Keyboard::KeyType>({ENABLE_KEY_LIST}) ) {
        if ( kbd.getKeyState(ch) == Keyboard::KeyState::Pressed ) {
            pressScanned_.push_back( static_cast<wchar_t>(ch) );
        }
        else if ( kbd.getKeyState(ch) == Keyboard::KeyState::Released ) {
            releaseScanned_.push_back( static_cast<wchar_t>(ch) );
        }
    }
}

void KeyboardTestWindow::initWindowClass()
{
    pWndClass.reset(new WindowClass);
}

LRESULT KeyboardTestWindow::handleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    try {
        outputKeyboardInput();

        switch (msg) {
        case WM_CLOSE:
            PostQuitMessage(69);
            break;

        case WM_KILLFOCUS:
            kbd.disableListening();
            break;

        case WM_SETFOCUS:
            kbd.enableListening();
            break;

        case WM_KEYDOWN: case WM_SYSKEYDOWN:
            if ( kbd.isListening() && kbd.isListeningKey( static_cast<
                Keyboard::KeyType>(wParam) )
                && ( kbd.getEventQueue().autoRepeatEnabled()
                    || !( lParam & BIT(30) ) )
            ) {
                kbd.getEventQueue().onKeyPressed( static_cast<
                    Keyboard::KeyType>(wParam) );                
            }

            if (wParam == VK_RETURN) {
                kbd.disableListening();
            }
            else if (wParam == VK_MENU) {
                kbd.getEventQueue().disableAutoRepeat();
            }
            break;

        case WM_KEYUP: case WM_SYSKEYUP:
            if ( kbd.isListening() && kbd.isListeningKey( static_cast<
                Keyboard::KeyType>(wParam) ) ) {
                kbd.getEventQueue().onKeyReleased( static_cast<
                    Keyboard::KeyType>(wParam) );                
            }

            if (wParam == VK_RETURN) {
                kbd.enableListening();
            }
            else if (wParam == VK_MENU) {
                kbd.getEventQueue().enableAutoRepeat();
            }

            break;

        case WM_CHAR:
            if ( kbd.isListening() && kbd.isListeningKey( static_cast<
                Keyboard::KeyType>(wParam) ) ) {
                kbd.getEventQueue().onChar( static_cast<
                    Keyboard::KeyType>(wParam) );                
            }
            break;

        case WM_LBUTTONDOWN:
            for ( auto ch : std::vector<Keyboard::KeyType>({ENABLE_KEY_LIST}) ) {
                kbd.ignoreKey(ch);
            }
            break;

        case WM_RBUTTONDOWN:
            for ( auto ch : std::vector<Keyboard::KeyType>({ENABLE_KEY_LIST}) ) {
                kbd.listenKey(ch);
            }
            break;
        }

    }   // try block
    catch (const Woon2Exception& e) {
    MessageBoxA(nullptr, e.what(), "Woon2 Exception",
        MB_OK | MB_ICONEXCLAMATION);
    }
    catch (const std::exception& e) {
        MessageBoxA(nullptr, e.what(), "Standard Exception",
            MB_OK | MB_ICONEXCLAMATION);
    }
    catch (...) {
        MessageBoxA(nullptr, "no details available",
            "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

#undef ENABLE_KEY_LIST