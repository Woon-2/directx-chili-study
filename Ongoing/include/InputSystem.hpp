#ifndef __InputSystem
#define __InputSystem

#include "InputComponent.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"

template <class CharT>
class InputSystem {
public:
    using MyChar = CharT;
    using MyKeyboard = Keyboard<MyChar>;
    using MyMouse = Mouse;

    void setListner(std::shared_ptr<IMouseInputComponent> listner) {
        ic_ = std::move(listner);
    }

    void update() {
        if ( auto ev = mouse_.peek(); ev.has_value() ) {
            ic_->receive(ev);
        }
    }

private:
    MyKeyboard kbd_;
    MyMouse mouse_;

    std::shared_ptr<IMouseInputComponent> ic_;
};

#endif  // __InputSystem