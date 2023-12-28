#ifndef __InputSystem
#define __InputSystem

#include "InputComponent.hpp"
#include "App/Keyboard.hpp"
#include "App/Mouse.hpp"

#include "App/ChiliWindow.hpp"

#include <type_traits>
#include <memory>
#include <optional>

struct AppMousePoint{
    float x;
    float y;

    AppMousePoint operator-() const noexcept {
        return AppMousePoint{ .x = -x, .y = -y };
    }

    AppMousePoint& operator+=(const AppMousePoint rhs) noexcept {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    AppMousePoint& operator-=(const AppMousePoint rhs) noexcept {
        return *this += -rhs;
    }

    friend AppMousePoint operator+(AppMousePoint lhs,
        const AppMousePoint rhs
    ) noexcept {
        return lhs += rhs;
    }

    friend AppMousePoint operator-(AppMousePoint lhs,
        const AppMousePoint rhs
    ) noexcept {
        return lhs -= rhs;
    }
};

// a class responsible for converting
// the mouse position represented in Window coordinate system
// to the mouse position represented in Application coordinate system.
class MousePointConverter {
public:
    MousePointConverter() = default;

    MousePointConverter(const Win32::Client& wndClient)
        : MousePointConverter(wndClient, -1.f, 1.f, 1.f, -1.f) {}

    MousePointConverter(float l, float t, float r, float b)
        : MousePointConverter( Win32::Client{}, l, t, r, b ) {}

    MousePointConverter(const Win32::Client& wndClient,
        float l, float t, float r, float b
    ) : client_(wndClient), left_(l), top_(t), right_(r), bottom_(b) {}

    AppMousePoint convert(const Mouse::Point& pt) const {
        return AppMousePoint{
            .x = ( pt.x - static_cast<int>(client_.x) ) / static_cast<float>(client_.width)
                * (right() - left()) + left(),
            .y = ( pt.y - static_cast<int>(client_.y) ) / static_cast<float>(client_.height)
                * (bottom() - top()) + top()
        };
    }

    void setClient(const Win32::Client& client) {
        client_ = client;
    }

    const Win32::Client& client() const noexcept {
        return client_;
    }

    void match(float left, float top, float right, float bottom) noexcept {
        setLeft(left);
        setTop(top);
        setRight(right);
        setBottom(bottom);
    }

    void setLeft(float left) noexcept {
        left_ = left;
    }

    void setTop(float top) noexcept {
        top_ = top;
    }

    void setRight(float right) noexcept {
        right_ = right;
    }

    void setBottom(float bottom) noexcept {
        bottom_ = bottom;
    }

    float left() const noexcept {
        return left_;
    }

    float top() const noexcept {
        return top_;
    }

    float right() const noexcept {
        return right_;
    }

    float bottom() const noexcept {
        return bottom_;
    }

private:
    Win32::Client client_;
    float left_;
    float top_;
    float right_;
    float bottom_;
};

template <class CharT>
class InputSystem {
public:
    using MyChar = CharT;
    using MyKeyboard = Keyboard<MyChar>;
    using MyMouse = Mouse;
    using MyIKbdIC = IKeyboardInputComponent<MyChar>;
    using MyIMsIC = IMouseInputComponent;

    InputSystem() = default;
    InputSystem(MyKeyboard& kbd, MyMouse& mouse, const Win32::Client& client)
        : mousePointConverter_(client), pKbd_(&kbd), pMouse_(&mouse),
        icMouse_(), icKbd_() {}

    void setListner(std::shared_ptr<MyIMsIC> listner) {
        icMouse_ = std::move(listner);
    }

    void setListner(std::shared_ptr<MyIKbdIC> listner) {
        icKbd_ = std::move(listner);
    }

    void plugin(MyKeyboard& kbd) {
        pKbd_ = &kbd;
    }

    void plugin(MyMouse& mouse) {
        pMouse_ = &mouse;
    }

    template <class T>
    void plugout() {
        using non_qualified_t = std::remove_cvref_t<T>;
        if constexpr ( std::is_base_of_v<MyKeyboard, non_qualified_t> ) {
            pKbd_.reset();
        }
        else if constexpr ( std::is_base_of_v<MyMouse, non_qualified_t> ) {
            pMouse_.reset();
        }
    }

    void update() {
        updateKbdRelated();
        updateMouseRelated();
        if (!pMouse_.has_value() || !icMouse_) {
            return;
        }

        if ( auto ev = pMouse_.value()->peek(); ev.has_value() ) {
            icMouse_->receive(ev.value());
        }
    }

    MousePointConverter& mousePointConverter() noexcept {
        return mousePointConverter_;
    }

    const MousePointConverter& mousePointConverter() const noexcept {
        return mousePointConverter_;
    }

private:
    void updateKbdRelated() {
        if (!pKbd_.has_value() || !icKbd_) {
            return;
        }

        if ( auto ev = pKbd_.value()->readKey(); ev.has_value() ) {
            icKbd_->receive(ev.value());
        }
    }

    void updateMouseRelated() {
        if (!pMouse_.has_value() || !icMouse_) {
            return;
        }

        if ( auto ev = pMouse_.value()->peek(); ev.has_value() ) {
            icMouse_->receive(ev.value());
        }
    }

    MousePointConverter mousePointConverter_;
    std::optional< MyKeyboard* > pKbd_;
    std::optional< MyMouse* > pMouse_;

    std::shared_ptr<MyIMsIC> icMouse_;
    std::shared_ptr<MyIKbdIC> icKbd_;
};

#endif  // __InputSystem