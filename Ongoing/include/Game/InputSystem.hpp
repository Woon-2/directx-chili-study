#ifndef __InputSystem
#define __InputSystem

#include "InputComponent.hpp"
#include "App/Keyboard.hpp"
#include "App/Mouse.hpp"

#include <type_traits>
#include <memory>
#include <optional>

template <class CharT>
class InputSystem {
public:
    using MyChar = CharT;
    using MyKeyboard = Keyboard<MyChar>;
    using MyMouse = Mouse;

    InputSystem() = default;
    InputSystem(MyKeyboard& kbd, MyMouse& mouse)
        : pKbd_(&kbd), pMouse_(&mouse) {}

    void setListner(std::shared_ptr<IMouseInputComponent> listner) {
        ic_ = std::move(listner);
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
        if (!pMouse_.has_value()) {
            return;
        }

        if ( auto ev = pMouse_.value()->peek(); ev.has_value() ) {
            ic_->receive(ev.value());
        }
    }

private:
    std::optional< MyKeyboard* > pKbd_;
    std::optional< MyMouse* > pMouse_;

    std::shared_ptr<IMouseInputComponent> ic_;
};

#endif  // __InputSystem