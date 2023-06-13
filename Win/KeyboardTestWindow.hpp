#ifndef __KeyboardTestWindow
#define __KeyboardTestWindow

#include "WindowBase.hpp"

#include <string>
#include <memory>

#include "Keyboard.hpp"

class KeyboardTestWindow : public WindowBase<KeyboardTestWindow, wchar_t>
{
private:
    class WindowClass;

public:
    friend class WindowBase;

    KeyboardTestWindow(const RECT& region);
    KeyboardTestWindow(const KeyboardTestWindow&) = delete;
    KeyboardTestWindow& operator=(const KeyboardTestWindow&) = delete;
    KeyboardTestWindow(KeyboardTestWindow&&) noexcept = delete;
    KeyboardTestWindow& operator=(KeyboardTestWindow&&) noexcept = delete;
    ~KeyboardTestWindow();

    const std::wstring& getTestOutput() const;
    void clearTestOutput();
    void outputKeyboardInput();
    std::wstring getScanned() const;
    void clearScanned();
    void scan();

    Keyboard kbd;

private:
    static std::unique_ptr<WindowClass> pWndClass;

    static void initWindowClass();
    LRESULT handleMsg( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

    RECT region_;
    HWND hWnd_;

    std::wstring testOutput_;
    std::wstring pressScanned_;
    std::wstring releaseScanned_;
};

class KeyboardTestWindow::WindowClass
{
public:
    WindowClass();
    ~WindowClass();
    WindowClass(const WindowClass&) = delete;
    WindowClass& operator=(const WindowClass&) = delete;
    WindowClass(WindowClass&&) noexcept = delete;
    WindowClass& operator=(WindowClass&&) noexcept = delete;

    HINSTANCE getInst() const;
    const std::wstring& getName() const;

private:
    std::wstring name_;
    HINSTANCE hInst_;
};

#endif  // __KeyboardTestWindow